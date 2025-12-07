#include "pch.h"
#include "Core/GameLauncher.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

namespace Core
{

    // --- Utilities ---
    static std::string Trim(const std::string &str)
    {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (std::string::npos == first)
            return str;
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }

    std::vector<std::string> GetAvailableDrives()
    {
        std::vector<std::string> drives;
#ifdef _WIN32
        DWORD mask = GetLogicalDrives();
        for (int i = 0; i < 26; ++i)
        {
            if (mask & (1 << i))
            {
                char driveName[] = {(char)('A' + i), ':', '\\', '\0'};
                drives.push_back(driveName);
            }
        }
#else
        drives.push_back("/");
#endif
        return drives;
    }

    // RAM Options
    static const int RAM_VALUES[] = {640, 1024, 4096, 8192, 16384, 32768, 65536, 131072, 262144};
    static const char *RAM_LABELS[] = {"640 KB", "1 MB", "4 MB", "8 MB", "16 MB", "32 MB", "64 MB", "128 MB", "256 MB"};

    // MIPS (CPU Speed) Presets
    // 0 for "Unlimited"
    struct MipsPreset
    {
        int value;
        const char *label;
    };
    static const MipsPreset MIPS_PRESETS[] = {
        {10, "10 MIPS (386 / Slow 486)"},
        {35, "35 MIPS (486 DX2-66)"},
        {60, "60 MIPS (Pentium 60 - DOS Default)"},
        {100, "100 MIPS (Pentium 90/100)"},
        {200, "200 MIPS (Pentium MMX)"},
        {0, "Unlimited (Windows Default)"}};
    static const int MIPS_PRESET_COUNT = sizeof(MIPS_PRESETS) / sizeof(MIPS_PRESETS[0]);

    // Resolution Presets (Windows Desktop Only)
    struct ResolutionPreset
    {
        int w;
        int h;
        int d;
        const char *label;
    };

    static const ResolutionPreset RES_PRESETS[] = {
        {640, 480, 8, "640 x 480 x 8 (256 Colors)"},
        {640, 480, 16, "640 x 480 x 16 (High Color)"},
        {640, 480, 32, "640 x 480 x 32 (True Color)"},
        {800, 600, 16, "800 x 600 x 16"},
        {800, 600, 32, "800 x 600 x 32 (SVGA)"},
        {1024, 768, 16, "1024 x 768 x 16"},
        {1024, 768, 32, "1024 x 768 x 32 (XGA)"},
        {1280, 1024, 32, "1280 x 1024 x 32 (SXGA)"},
        {1600, 1200, 32, "1600 x 1200 x 32 (UXGA)"}};

    // --- Lifecycle ---
    GameLauncher::GameLauncher()
    {
        std::error_code ec;
        m_browserCurrentPath = fs::current_path(ec);
        if (ec)
            m_browserCurrentPath = "C:\\";
        m_lastGlobalPath = m_browserCurrentPath;

        m_showEditWindow = false;
        m_showFileBrowser = false;
        m_pendingBrowserOpen = false;
        m_selectedGameIdx = -1;
    }

    GameLauncher::~GameLauncher()
    {
        SaveDatabase();
    }

    void GameLauncher::Initialize()
    {
        LoadDatabase();
    }

    // --- Helper: Background Directory Scanner ---
    // This runs on a separate thread to prevent UI freezing
    std::vector<Core::GameLauncher::FileBrowserEntry> ScanDirectoryAsync(
        fs::path path,
        std::vector<std::string> extensions)
    {
        std::vector<Core::GameLauncher::FileBrowserEntry> entries;
        try
        {
            for (const auto &entry : fs::directory_iterator(path, fs::directory_options::skip_permission_denied))
            {
                bool isDir = entry.is_directory();

                // Filter Logic (Same as before)
                if (!isDir && !extensions.empty())
                {
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                    bool match = false;
                    for (const auto &allowed : extensions)
                    {
                        if (ext == allowed)
                        {
                            match = true;
                            break;
                        }
                    }
                    if (!match)
                        continue;
                }

                Core::GameLauncher::FileBrowserEntry e;
                e.fullPath = entry.path();
                e.name = entry.path().filename().string();
                e.isDirectory = isDir;
                entries.push_back(e);
            }

            // Sort Logic
            std::sort(entries.begin(), entries.end(),
                      [](const Core::GameLauncher::FileBrowserEntry &a, const Core::GameLauncher::FileBrowserEntry &b)
                      {
                          if (a.isDirectory != b.isDirectory)
                              return a.isDirectory;
                          std::string s1 = a.name;
                          std::string s2 = b.name;
                          std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
                          std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
                          return s1 < s2;
                      });
        }
        catch (...)
        {
        }
        return entries;
    }

    // --- Helper: Sort Library ---
    void GameLauncher::SortLibrary()
    {
        if (m_games.empty())
            return;

        std::string selectedName = "";
        if (m_selectedGameIdx >= 0 && m_selectedGameIdx < (int)m_games.size())
        {
            selectedName = m_games[m_selectedGameIdx].name;
        }

        std::sort(m_games.begin(), m_games.end(), [](const GameEntry &a, const GameEntry &b)
                  {
        std::string s1 = a.name;
        std::string s2 = b.name;
        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        return s1 < s2; });

        m_selectedGameIdx = -1;
        if (!selectedName.empty())
        {
            for (size_t i = 0; i < m_games.size(); ++i)
            {
                if (m_games[i].name == selectedName)
                {
                    m_selectedGameIdx = (int)i;
                    break;
                }
            }
        }
    }

    // --- Persistence ---
    void GameLauncher::SaveDatabase()
    {
        SortLibrary();

        std::ofstream file("games_db.txt");
        if (!file.is_open())
            return;

        // Dreamm Path
        file << m_dreammExePath << "\n";

        // Last Selected Game Name
        std::string lastSelected = "";
        if (m_selectedGameIdx >= 0 && m_selectedGameIdx < (int)m_games.size())
        {
            lastSelected = m_games[m_selectedGameIdx].name;
        }
        file << lastSelected << "\n";

        for (const auto &game : m_games)
        {
            int audioMask = 0;
            for (int i = 0; i < 6; i++)
            {
                if (game.audioFlags[i])
                    audioMask |= (1 << i);
            }

            file << game.name << "|"
                 << game.exePath << "|"
                 << game.setupPath << "|"
                 << (int)game.platform << "|"
                 << (int)game.status << "|"
                 << game.ramKB << "|"
                 << audioMask << "|"
                 << game.videoHwIdx << "|"
                 << game.width << "|"
                 << game.height << "|"
                 << game.depth << "|"
                 << game.mips << "|"
                 << (int)game.machine << "|"
                 << game.description << "\n";
        }
        file.close();
    }

    void GameLauncher::LoadDatabase()
    {
        std::ifstream file("games_db.txt");
        if (!file.is_open())
            return;

        std::string line;

        // Load Path
        if (std::getline(file, line))
            m_dreammExePath = Trim(line);

        // Load Last Selected Name
        std::string lastSelectedName = "";
        if (std::getline(file, line))
            lastSelectedName = Trim(line);

        m_games.clear();
        while (std::getline(file, line))
        {
            line = Trim(line);
            if (line.empty())
                continue;

            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> seglist;

            while (std::getline(ss, segment, '|'))
            {
                seglist.push_back(segment);
            }

            if (seglist.size() >= 4)
            {
                GameEntry g;
                g.name = seglist[0];
                g.exePath = seglist[1];
                g.setupPath = seglist[2];
                try
                {
                    g.platform = (GamePlatform)std::stoi(seglist[3]);
                    g.status = (GameStatus)std::stoi(seglist[4]);
                    if (seglist.size() > 5)
                        g.ramKB = std::stoi(seglist[5]);
                    if (seglist.size() > 6)
                    {
                        int audioMask = std::stoi(seglist[6]);
                        for (int i = 0; i < 6; i++)
                            g.audioFlags[i] = (audioMask & (1 << i)) != 0;
                    }
                    if (seglist.size() > 7)
                        g.videoHwIdx = std::stoi(seglist[7]);
                    if (seglist.size() > 10)
                    {
                        g.width = std::stoi(seglist[8]);
                        g.height = std::stoi(seglist[9]);
                        g.depth = std::stoi(seglist[10]);
                    }
                    if (seglist.size() > 11)
                        g.mips = std::stoi(seglist[11]);
                    if (seglist.size() > 12)
                        g.machine = (MachineType)std::stoi(seglist[12]);
                    if (seglist.size() > 13)
                        g.description = seglist[13];
                }
                catch (...)
                {
                }
                m_games.push_back(g);
            }
        }

        // Sort first, then find the index
        SortLibrary();

        if (!lastSelectedName.empty())
        {
            for (size_t i = 0; i < m_games.size(); ++i)
            {
                if (m_games[i].name == lastSelectedName)
                {
                    m_selectedGameIdx = (int)i;
                    m_scrollToSelection = true; // Trigger scroll on next render
                    break;
                }
            }
        }
    }

    // --- Launch Logic ---
    std::string GameLauncher::BuildCommand(const GameEntry &game, bool runSetup)
    {
        if (m_dreammExePath.empty())
            return "";

        std::string targetExe = runSetup ? game.setupPath : game.exePath;
        if (targetExe.empty())
            return "";

        fs::path targetPath(targetExe);
        if (!fs::exists(targetPath))
            return "";

        std::string cmd = "\"" + m_dreammExePath + "\"";

        // Mount C: (Always mount the folder containing the executable)
        cmd += " -mount rw:c=\"" + targetPath.parent_path().string() + "\"";

        // Mount D: (If an ISO/CUE is provided)
        if (!game.isoPath.empty() && fs::exists(game.isoPath))
        {
            cmd += " -mount \"d=" + game.isoPath + "\"";
        }

        if (!game.rootPathOverride.empty())
            cmd += " -prop rootpath=\"" + game.rootPathOverride + "\"";

        cmd += " -prop ramkb=" + std::to_string(game.ramKB);
        // Handle 0 as "unlimited"
        if (game.mips <= 0)
        {
            cmd += " -prop mips=unlimited";
        }
        else
        {
            cmd += " -prop mips=" + std::to_string(game.mips);
        }
        cmd += " -prop machine=" + std::string(game.machine == MachineType::PC ? "pc" : "tandy");

        std::string audioStr = "";
        for (int i = 0; i < 6; i++)
        {
            if (game.audioFlags[i])
            {
                if (!audioStr.empty())
                    audioStr += "+";
                audioStr += m_audioNames[i];
            }
        }
        if (audioStr.empty())
            audioStr = "sb16";
        cmd += " -prop audiohw=" + audioStr;

        cmd += " -prop videohw=" + std::string(m_videoHwOptions[game.videoHwIdx]);
        cmd += " -prop winres=" + std::to_string(game.width) + "x" + std::to_string(game.height) + "x" + std::to_string(game.depth);
        cmd += " -launch \"" + targetExe + "\"";

        return cmd;
    }

    void GameLauncher::CreateDreammFile(const GameEntry &game)
    {
        if (m_dreammExePath.empty() || game.exePath.empty())
            return;

        fs::path targetPath(game.exePath);
        if (!fs::exists(targetPath))
            return;

        std::string cmd = "\"" + m_dreammExePath + "\"";

        // Mounts
        cmd += " -mount rw:c=\"" + targetPath.parent_path().string() + "\"";
        if (!game.isoPath.empty() && fs::exists(game.isoPath))
        {
            cmd += " -mount \"d=" + game.isoPath + "\"";
        }

        // Properties
        if (!game.rootPathOverride.empty())
            cmd += " -prop rootpath=\"" + game.rootPathOverride + "\"";
        cmd += " -prop ramkb=" + std::to_string(game.ramKB);
        cmd += " -prop mips=" + std::to_string(game.mips);
        cmd += " -prop machine=" + std::string(game.machine == MachineType::PC ? "pc" : "tandy");

        std::string audioStr = "";
        for (int i = 0; i < 6; i++)
        {
            if (game.audioFlags[i])
            {
                if (!audioStr.empty())
                    audioStr += "+";
                audioStr += m_audioNames[i];
            }
        }
        if (audioStr.empty())
            audioStr = "sb16";
        cmd += " -prop audiohw=" + audioStr;
        cmd += " -prop videohw=" + std::string(m_videoHwOptions[game.videoHwIdx]);
        cmd += " -prop win32res=" + std::to_string(game.width) + "x" + std::to_string(game.height) + "x" + std::to_string(game.depth);

        // The filename will be "GameEXE.dreamm" (e.g., DOOM.EXE -> DOOM.dreamm)
        std::string dreammFileName = targetPath.filename().stem().string() + ".dreamm";
        cmd += " -makedream \"" + dreammFileName + "\"";

        // Launch
        cmd += " -launch \"" + game.exePath + "\"";

        SDL_Log("Creating DREAMM File: %s", cmd.c_str());

#ifdef _WIN32
        std::string params = cmd.substr(m_dreammExePath.length() + 2);
        ShellExecuteA(NULL, "open", m_dreammExePath.c_str(), params.c_str(), NULL, SW_SHOWDEFAULT);
#else
        std::system(cmd.c_str());
#endif
    }

    void GameLauncher::LaunchGame(const GameEntry &game, bool runSetup)
    {
        std::string cmd = BuildCommand(game, runSetup);
        if (cmd.empty())
            return;

        SDL_Log("Launching: %s", cmd.c_str());
        SaveDatabase();

        // Hide the Launcher Window
        SDL_Window *window = SDL_GL_GetCurrentWindow();
        if (window)
            SDL_HideWindow(window);

        // Execute and BLOCK until finished
#ifdef _WIN32
        std::string params = "";
        if (cmd.length() > m_dreammExePath.length() + 2)
        {
            params = cmd.substr(m_dreammExePath.length() + 2);
        }

        SHELLEXECUTEINFOA shExInfo = {0};
        shExInfo.cbSize = sizeof(shExInfo);
        shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS; // CRITICAL: Ask for the process handle
        shExInfo.hwnd = NULL;
        shExInfo.lpVerb = "open";
        shExInfo.lpFile = m_dreammExePath.c_str();
        shExInfo.lpParameters = params.c_str();
        shExInfo.lpDirectory = NULL;
        shExInfo.nShow = SW_SHOWDEFAULT;

        if (ShellExecuteExA(&shExInfo))
        {
            WaitForSingleObject(shExInfo.hProcess, INFINITE);
            CloseHandle(shExInfo.hProcess);
        }
#else
        // Linux/Mac: std::system is naturally blocking
        std::system(cmd.c_str());
#endif

        // Game Closed - Restore Launcher
        if (window)
        {
            SDL_ShowWindow(window);
            SDL_RaiseWindow(window); 
        }
    }

    // --- Browser Logic ---
    void GameLauncher::OpenFileBrowser(const std::string &title, std::string *targetStringRef, const std::vector<std::string> &allowedExtensions)
    {
        m_showFileBrowser = true;
        m_fileBrowserTitle = title;
        m_fileBrowserTarget = targetStringRef;
        m_fileBrowserExtensions = allowedExtensions;

        // Determine Path
        bool pathFound = false;
        if (targetStringRef && !targetStringRef->empty())
        {
            fs::path existingPath(*targetStringRef);
            std::error_code ec;
            if (fs::exists(existingPath, ec))
            {
                if (fs::is_directory(existingPath, ec))
                    m_browserCurrentPath = existingPath;
                else
                    m_browserCurrentPath = existingPath.parent_path();
                pathFound = true;
            }
            else if (existingPath.has_parent_path() && fs::exists(existingPath.parent_path(), ec))
            {
                m_browserCurrentPath = existingPath.parent_path();
                pathFound = true;
            }
        }

        if (!pathFound)
        {
            std::error_code ec;
            if (!fs::exists(m_lastGlobalPath, ec))
                m_lastGlobalPath = "C:\\";
            m_browserCurrentPath = m_lastGlobalPath;
        }

        m_browserEntries.clear();

        m_loadingFuture = std::async(std::launch::async, ScanDirectoryAsync, m_browserCurrentPath, m_fileBrowserExtensions);

        m_pendingBrowserOpen = true;
    }

    void GameLauncher::RenderFileBrowser()
    {
        if (!m_showFileBrowser)
            return;

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(600, 450));

        if (ImGui::BeginPopupModal("File Browser", NULL, ImGuiWindowFlags_NoResize))
        {
            ImGui::Text("Select File for: %s", m_fileBrowserTitle.c_str());
            ImGui::Separator();

            bool isLoading = false;

            bool triggerScrollCalculation = false;

            if (m_loadingFuture.valid())
            {
                if (m_loadingFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                    m_browserEntries = m_loadingFuture.get();
                    // Data is fresh. If we have a target, trigger the scroll logic.
                    if (!m_autoScrollTarget.empty())
                        triggerScrollCalculation = true;
                }
                else
                {
                    isLoading = true;
                }
            }

            const bool uiDisabled = isLoading;

            // --- Drive Selection ---
            if (uiDisabled)
                ImGui::BeginDisabled();

            ImGui::Text("Drive:");
            ImGui::SameLine();
            std::vector<std::string> drives = GetAvailableDrives();
            std::string currentRoot = m_browserCurrentPath.root_path().string();
            const char *previewValue = currentRoot.c_str();

            ImGui::PushItemWidth(100);
            if (ImGui::BeginCombo("##drive_combo", previewValue))
            {
                for (const auto &drive : drives)
                {
                    bool isSelected = (currentRoot.find(drive) != std::string::npos);
                    if (ImGui::Selectable(drive.c_str(), isSelected))
                    {
                        m_browserCurrentPath = drive;
                        m_lastGlobalPath = m_browserCurrentPath;

                        m_autoScrollTarget.clear(); // Reset scroll on drive change
                        m_browserEntries.clear();
                        m_loadingFuture = std::async(std::launch::async, ScanDirectoryAsync, m_browserCurrentPath, m_fileBrowserExtensions);
                        isLoading = true;
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();

            if (uiDisabled)
                ImGui::EndDisabled();

            ImGui::SameLine();
            ImGui::Text(" | %s", m_browserCurrentPath.string().c_str());

            // --- Navigation ---
            bool isRoot = (m_browserCurrentPath == m_browserCurrentPath.root_path());

            if (uiDisabled || isRoot)
                ImGui::BeginDisabled();

            if (ImGui::Button("^ Up"))
            {
                if (!isRoot)
                {
                    m_autoScrollTarget = m_browserCurrentPath.filename().string();

                    m_browserCurrentPath = m_browserCurrentPath.parent_path();
                    m_lastGlobalPath = m_browserCurrentPath;

                    m_browserEntries.clear();
                    m_loadingFuture = std::async(std::launch::async, ScanDirectoryAsync, m_browserCurrentPath, m_fileBrowserExtensions);
                    isLoading = true;
                }
            }

            if (uiDisabled || isRoot)
                ImGui::EndDisabled();

            ImGui::Separator();

            // --- File List ---
            ImGui::BeginChild("Files", ImVec2(0, -40), true);

            // --- Auto Scroll Logic ---
            if (triggerScrollCalculation && !m_browserEntries.empty())
            {

                int targetIndex = -1;

                // Find index of the folder we just left
                for (int i = 0; i < (int)m_browserEntries.size(); i++)
                {
                    if (m_browserEntries[i].name == m_autoScrollTarget)
                    {
                        targetIndex = i;
                        break;
                    }
                }

                if (targetIndex != -1)
                {
                    // Adjust index for the ".." item at the top (if exists)
                    if (!isRoot)
                        targetIndex += 1;

                    // Calculate pixel height (Selectable height usually TextHeight + Spacing)
                    float itemHeight = ImGui::GetTextLineHeightWithSpacing();

                    // Center the item in the window
                    // ScrollY = (ItemPos) - (HalfWindowHeight)
                    float scrollY = (targetIndex * itemHeight) - (ImGui::GetWindowHeight() * 0.5f);

                    ImGui::SetScrollY(scrollY);
                }

                m_autoScrollTarget.clear(); // Clear target so we don't scroll again
            }
            // ------------------------------------

            if (isLoading)
            {
                ImVec2 windowCenter = ImGui::GetWindowSize();
                windowCenter.x *= 0.5f;
                windowCenter.y *= 0.5f;
                ImGui::SetCursorPos(ImVec2(windowCenter.x - 40, windowCenter.y - 10));
                ImGui::Text("Reading Folder...");
            }
            else
            {
                // Parent Directory (..) Item
                if (!isRoot)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.5f, 1.0f));
                    if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick))
                    {
                        if (ImGui::IsMouseDoubleClicked(0))
                        {
                            m_autoScrollTarget = m_browserCurrentPath.filename().string();

                            m_browserCurrentPath = m_browserCurrentPath.parent_path();
                            m_lastGlobalPath = m_browserCurrentPath;

                            m_browserEntries.clear();
                            m_loadingFuture = std::async(std::launch::async, ScanDirectoryAsync, m_browserCurrentPath, m_fileBrowserExtensions);

                            ImGui::PopStyleColor();
                            ImGui::EndChild();
                            ImGui::EndPopup();
                            return;
                        }
                    }
                    ImGui::PopStyleColor();
                }

                // File Items
                ImGuiListClipper clipper;
                clipper.Begin((int)m_browserEntries.size());

                while (clipper.Step())
                {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                    {
                        if (i < 0 || i >= (int)m_browserEntries.size())
                            break;

                        const auto &entry = m_browserEntries[i];

                        if (entry.isDirectory)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.5f, 1.0f));
                            if (ImGui::Selectable(("[DIR] " + entry.name).c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                            {
                                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                                {
                                    std::error_code ec;
                                    if (fs::exists(entry.fullPath, ec))
                                    {
                                        m_browserCurrentPath = entry.fullPath;
                                        m_lastGlobalPath = m_browserCurrentPath;

                                        // Reset scroll target (we are going deeper, start at top)
                                        m_autoScrollTarget.clear();

                                        m_browserEntries.clear();
                                        m_loadingFuture = std::async(std::launch::async, ScanDirectoryAsync, m_browserCurrentPath, m_fileBrowserExtensions);

                                        ImGui::PopStyleColor();
                                        ImGui::EndChild();
                                        ImGui::EndPopup();
                                        return;
                                    }
                                }
                            }
                            ImGui::PopStyleColor();
                        }
                        else
                        {
                            if (ImGui::Selectable(entry.name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                            {
                                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                                {
                                    if (m_fileBrowserTarget)
                                    {
                                        *m_fileBrowserTarget = entry.fullPath.string();
                                        if (m_selectedGameIdx >= 0 && m_selectedGameIdx < (int)m_games.size())
                                        {
                                            if (m_fileBrowserTarget == &m_games[m_selectedGameIdx].exePath)
                                            {
                                                if (m_games[m_selectedGameIdx].name == "New Game")
                                                {
                                                    std::string parentName = entry.fullPath.parent_path().filename().string();
                                                    if (!parentName.empty())
                                                        m_games[m_selectedGameIdx].name = parentName;
                                                }
                                            }
                                        }
                                    }
                                    m_lastGlobalPath = m_browserCurrentPath;
                                    m_showFileBrowser = false;
                                    ImGui::CloseCurrentPopup();
                                }
                            }
                        }
                    }
                }
            }

            ImGui::EndChild();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                m_showFileBrowser = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    // --- UI Rendering ---

    void GameLauncher::RenderGameList()
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.12f, 0.85f));

        ImGui::BeginChild("LeftColumnChild", ImVec2(0, 0), false);

        ImGui::TextDisabled("LIBRARY FILTER");
        ImGui::InputTextWithHint("##filter", "Name search...", m_filterName, 256);
        ImGui::Combo("Platform", &m_filterPlatform, "All\0DOS\0Windows\0\0");
        ImGui::Combo("Status", &m_filterStatus, "All\0Unplayable\0Playable\0\0");
        ImGui::Separator();

        // Start List
        ImGui::BeginChild("GameListScroll", ImVec2(0, -40), false);

        for (size_t i = 0; i < m_games.size(); ++i)
        {
            const auto &g = m_games[i];

            // Filter Logic
            if (strlen(m_filterName) > 0)
            {
                std::string n = g.name;
                if (n.find(m_filterName) == std::string::npos)
                    continue;
            }
            if (m_filterPlatform > 0)
            {
                if (m_filterPlatform == 1 && g.platform != GamePlatform::DOS)
                    continue;
                if (m_filterPlatform == 2 && g.platform != GamePlatform::Windows)
                    continue;
            }
            if (m_filterStatus > 0)
            {
                if (m_filterStatus == 1 && g.status != GameStatus::Unplayable)
                    continue;
                if (m_filterStatus == 2 && g.status != GameStatus::Playable)
                    continue;
            }

            bool isPlayable = (g.status == GameStatus::Playable);
            if (!isPlayable)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

            std::string label = g.name + "##" + std::to_string(i);

            // Selectable Item
            if (ImGui::Selectable(label.c_str(), m_selectedGameIdx == (int)i, ImGuiSelectableFlags_AllowDoubleClick))
            {
                m_selectedGameIdx = (int)i;
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    if (!g.exePath.empty())
                        LaunchGame(g, false);
                }
            }

            // Run this check if we have pending scroll frames
            if (m_autoScrollFrames > 0 && m_selectedGameIdx == (int)i)
            {
                // 1.0f means "Align this item to the bottom of the visible view"
                // This ensures we scroll DOWN to see it.
                ImGui::SetScrollHereY(1.0f);
            }

            if (!isPlayable)
                ImGui::PopStyleColor();
        }

        // Decrement frame counter (keeps scroll active for 2 frames to catch layout updates)
        if (m_autoScrollFrames > 0)
            m_autoScrollFrames--;

        ImGui::EndChild();

        // --- ADD GAME BUTTON ---
        if (ImGui::Button("+ Add Game", ImVec2(-1, 30)))
        {
            // Clear filters so the new item is definitely visible
            m_filterName[0] = '\0';
            m_filterPlatform = 0;
            m_filterStatus = 0;

            // Add the game
            GameEntry g;
            g.name = "New Game";
            g.platform = GamePlatform::DOS;
            g.ramKB = 640;
            m_games.push_back(g);

            m_selectedGameIdx = (int)m_games.size() - 1;

            m_autoScrollFrames = 2;

            m_showEditWindow = true;
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void GameLauncher::RenderGameDashboard()
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.12f, 0.3f));

        ImGui::BeginChild("RightColumnChild", ImVec2(0, 0), false);

        if (m_selectedGameIdx < 0 || m_selectedGameIdx >= (int)m_games.size())
        {
            ImGui::TextDisabled("Select a game from the library.");
            ImGui::EndChild();
            ImGui::PopStyleColor();
            return;
        }

        GameEntry &game = m_games[m_selectedGameIdx];

        // --- Title ---
        bool hasLargeFont = (ImGui::GetIO().Fonts->Fonts.Size > 1);
        if (hasLargeFont)
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text("%s", game.name.c_str());
        if (hasLargeFont)
            ImGui::PopFont();

        // --- Status ---
        ImGui::SameLine();
        if (game.status == GameStatus::Playable)
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "[PLAYABLE]");
        else
            ImGui::TextColored(ImVec4(1, 0.5, 0, 1), "[UNPLAYABLE]");

        ImGui::Spacing();

        bool canPlay = !game.exePath.empty();
        if (!canPlay)
            ImGui::BeginDisabled();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));

        // PLAY BUTTON

        if (ImGui::Button("PLAY", ImVec2(150, 50)))
        {
            LaunchGame(game, false);
        }

        ImGui::PopStyleColor(2);

        if (!canPlay)
            ImGui::EndDisabled();

        ImGui::SameLine();

        // SETUP BUTTON

        if (game.setupPath.empty())
            ImGui::BeginDisabled();
        if (ImGui::Button("Run Setup", ImVec2(100, 50)))
        {
            LaunchGame(game, true);
        }
        if (game.setupPath.empty())
            ImGui::EndDisabled();

        ImGui::SameLine();

        // SETTINGS BUTTON

        if (ImGui::Button("Settings", ImVec2(100, 50)))
        {
            m_showEditWindow = true;
        }

        ImGui::SameLine();

        // MAKE DREAMM BUTTON

        if (!canPlay)
            ImGui::BeginDisabled();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.7f, 0.9f, 1.0f));
        if (ImGui::Button("Make\nDREAMM", ImVec2(80, 50)))
        {
            CreateDreammFile(game);
        }
        ImGui::PopStyleColor(2);
        if (!canPlay)
            ImGui::EndDisabled();

        ImGui::Separator();

        ImGui::TextDisabled("DESCRIPTION");
        ImGui::TextWrapped("%s", game.description.empty() ? "No description." : game.description.c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextDisabled("INFO");
        ImGui::Text("Platform: %s", game.platform == GamePlatform::DOS ? "DOS" : "Windows");
        ImGui::Text("Machine: %s", game.machine == MachineType::PC ? "PC" : "Tandy");
        ImGui::Text("EXE: %s", game.exePath.c_str());

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void GameLauncher::RenderEditWindow()
    {
        if (!m_showEditWindow)
            return;

        // If index is invalid, force close the window to prevent crash
        if (m_selectedGameIdx < 0 || m_selectedGameIdx >= (int)m_games.size())
        {
            m_showEditWindow = false;
            return;
        }

        // Ensure Parent Popup is Open
        if (m_showEditWindow && !ImGui::IsPopupOpen("Edit Game Details"))
        {
            ImGui::OpenPopup("Edit Game Details");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);

        // Begin Parent Modal
        if (ImGui::BeginPopupModal("Edit Game Details", &m_showEditWindow, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse))
        {

            if (m_selectedGameIdx >= 0 && m_selectedGameIdx < (int)m_games.size())
            {
                GameEntry &game = m_games[m_selectedGameIdx];

                if (ImGui::BeginTable("EditForm", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);

                    // --- Basic Info ---
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Game Title");
                    ImGui::TableSetColumnIndex(1);

                    char nameBuf[256] = {0};
                    strncpy(nameBuf, game.name.c_str(), sizeof(nameBuf) - 1);
                    if (ImGui::InputText("##Name", nameBuf, sizeof(nameBuf)))
                        game.name = nameBuf;

                    if (ImGui::IsItemHovered())
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Description");
                    ImGui::TableSetColumnIndex(1);

                    char descBuf[4096] = {0};
                    strncpy(descBuf, game.description.c_str(), sizeof(descBuf) - 1);
                    if (ImGui::InputTextMultiline("##Desc", descBuf, sizeof(descBuf), ImVec2(-1, 100)))
                        game.description = descBuf;

                    if (ImGui::IsItemHovered())
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Platform");
                    ImGui::TableSetColumnIndex(1);
                    int platform = (int)game.platform;
                    if (ImGui::Combo("##Platform", &platform, "DOS\0Windows\0\0"))
                    {
                        game.platform = (GamePlatform)platform;
                        for (int i = 0; i < 6; i++)
                            game.audioFlags[i] = false;
                        if (game.platform == GamePlatform::Windows)
                        {
                            game.ramKB = 65536;
                            game.audioFlags[5] = true;
                        }
                        else
                        {
                            game.ramKB = 640;
                            game.audioFlags[3] = true;
                            game.audioFlags[5] = true;
                        }
                    }

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Status");
                    ImGui::TableSetColumnIndex(1);
                    int status = (int)game.status;
                    if (ImGui::Combo("##Status", &status, "Unplayable\0Playable\0\0"))
                        game.status = (GameStatus)status;

                    // --- Paths ---
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Game EXE");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##ExePath", (char *)game.exePath.c_str(), game.exePath.size() + 1, ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::IsItemHovered())
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
                    ImGui::SameLine();
                    if (ImGui::Button("Browse##ExeBtn"))
                        OpenFileBrowser("Game Executable", &game.exePath, {".exe", ".bat", ".com", ".vhd"});

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Setup EXE");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##SetPath", (char *)game.setupPath.c_str(), game.setupPath.size() + 1, ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::IsItemHovered())
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
                    ImGui::SameLine();
                    if (ImGui::Button("Browse##SetBtn"))
                        OpenFileBrowser("Setup Executable", &game.setupPath, {".exe", ".bat", ".com"});

                    // --- CD Mount Area ---
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("CD Image (D:)");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##IsoPath", (char *)game.isoPath.c_str(), game.isoPath.size(), ImGuiInputTextFlags_ReadOnly);
                    if (ImGui::IsItemHovered())
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
                    ImGui::SameLine();
                    if (ImGui::Button("Browse##IsoBtn"))
                        OpenFileBrowser("CD Image", &game.isoPath, {".iso", ".cue", ".bin", ".img", ".ccd"});

                    ImGui::EndTable();
                }

                ImGui::Separator();

                if (ImGui::BeginTabBar("EditTabs"))
                {
                    // --- MACHINE TAB with RAM COMBO ---
                    if (ImGui::BeginTabItem("Machine"))
                    {
                        ImGui::BeginTable("MachTable", 2, ImGuiTableFlags_SizingStretchProp);
                        ImGui::TableSetupColumn("L", ImGuiTableColumnFlags_WidthFixed, 100);
                        ImGui::TableSetupColumn("I");

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("RAM Size");
                        ImGui::TableSetColumnIndex(1);

                        // Logic to find current index matching kb, or custom
                        int currentRamIdx = -1;
                        for (int i = 0; i < 9; i++)
                        {
                            if (game.ramKB == RAM_VALUES[i])
                            {
                                currentRamIdx = i;
                                break;
                            }
                        }

                        if (ImGui::Combo("##RAMCombo", &currentRamIdx, RAM_LABELS, 9))
                        {
                            if (currentRamIdx >= 0)
                                game.ramKB = RAM_VALUES[currentRamIdx];
                        }
                        if (currentRamIdx == -1)
                        {
                            ImGui::SameLine();
                            ImGui::Text("(Custom: %d KB)", game.ramKB);
                        }

                        // --- MIPS (CPU Speed) ---
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("CPU Speed");
                        ImGui::TableSetColumnIndex(1);

                        // Find Current Preset
                        int currentMipsIdx = -1;
                        for (int i = 0; i < MIPS_PRESET_COUNT; i++)
                        {
                            if (game.mips == MIPS_PRESETS[i].value)
                            {
                                currentMipsIdx = i;
                                break;
                            }
                        }

                        ImGui::PushItemWidth(200);
                        const char *comboPreview = (currentMipsIdx != -1) ? MIPS_PRESETS[currentMipsIdx].label : "Custom";
                        if (ImGui::BeginCombo("##MipsCombo", comboPreview))
                        {
                            for (int n = 0; n < MIPS_PRESET_COUNT; n++)
                            {
                                const bool is_selected = (currentMipsIdx == n);
                                if (ImGui::Selectable(MIPS_PRESETS[n].label, is_selected))
                                {
                                    game.mips = MIPS_PRESETS[n].value;
                                }
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::PopItemWidth();

                        // MIPS Input
                        ImGui::SameLine();
                        ImGui::PushItemWidth(100);
                        ImGui::InputInt("##MipsInput", &game.mips, 0, 0);
                        if (ImGui::IsItemHovered())
                            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
                        ImGui::PopItemWidth();

                        // Help Tooltip
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::SetTooltip("0 = Unlimited (Windows)\n60 = Standard DOS");
                        }

                        // --- Machine Type ---
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Type");
                        ImGui::TableSetColumnIndex(1);
                        int mach = (int)game.machine;
                        if (ImGui::RadioButton("PC", mach == 0))
                            game.machine = MachineType::PC;
                        ImGui::SameLine();
                        if (ImGui::RadioButton("Tandy", mach == 1))
                            game.machine = MachineType::Tandy;

                        ImGui::EndTable();
                        ImGui::EndTabItem();
                    }

                    // --- VIDEO TAB with RESOLUTION COMBO ---
                    if (ImGui::BeginTabItem("Video"))
                    {
                        ImGui::BeginTable("VidTable", 2, ImGuiTableFlags_SizingStretchProp);
                        ImGui::TableSetupColumn("L", ImGuiTableColumnFlags_WidthFixed, 100);
                        ImGui::TableSetupColumn("I");

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Hardware");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Combo("##VidHW", &game.videoHwIdx, m_videoHwOptions, 6);

                        // --- WINDOWS RESOLUTION LOGIC ---
                        // Only enable these controls if Platform is Windows
                        bool isWindows = (game.platform == GamePlatform::Windows);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Desktop Res");
                        if (!isWindows && ImGui::IsItemHovered())
                            ImGui::SetTooltip("Only applicable for Windows guests.\nDOS handles resolution natively.");

                        ImGui::TableSetColumnIndex(1);

                        if (!isWindows)
                        {
                            ImGui::BeginDisabled();
                            ImGui::TextDisabled("Native (DOS)");
                        }
                        else
                        {
                            int curResIdx = -1;
                            int numRes = sizeof(RES_PRESETS) / sizeof(RES_PRESETS[0]);
                            const char *preview = "Custom";

                            for (int n = 0; n < numRes; n++)
                            {
                                if (game.width == RES_PRESETS[n].w &&
                                    game.height == RES_PRESETS[n].h &&
                                    game.depth == RES_PRESETS[n].d)
                                {
                                    curResIdx = n;
                                    preview = RES_PRESETS[n].label;
                                    break;
                                }
                            }

                            if (ImGui::BeginCombo("##ResCombo", preview))
                            {
                                for (int n = 0; n < numRes; n++)
                                {
                                    const bool is_selected = (curResIdx == n);
                                    if (ImGui::Selectable(RES_PRESETS[n].label, is_selected))
                                    {
                                        game.width = RES_PRESETS[n].w;
                                        game.height = RES_PRESETS[n].h;
                                        game.depth = RES_PRESETS[n].d;
                                    }
                                    if (is_selected)
                                        ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                        }

                        // Custom Inputs
                        if (isWindows)
                        {
                            ImGui::SameLine();
                            ImGui::Text("(%dx%d)", game.width, game.height);
                        }

                        // Manual Depth Selector (Only show if Windows)
                        if (isWindows)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Color Depth");
                            ImGui::TableSetColumnIndex(1);

                            int currentDepth = game.depth;
                            if (ImGui::BeginCombo("##DepthCombo", (std::to_string(currentDepth) + "-bit").c_str()))
                            {
                                int depths[] = {8, 16, 24, 32};
                                for (int d : depths)
                                {
                                    bool is_selected = (currentDepth == d);
                                    if (ImGui::Selectable((std::to_string(d) + "-bit").c_str(), is_selected))
                                    {
                                        game.depth = d;
                                    }
                                    if (is_selected)
                                        ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                        }

                        if (!isWindows)
                            ImGui::EndDisabled(); // End Grey out

                        ImGui::EndTable();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Audio"))
                    {
                        ImGui::Text("Select Active Audio Devices:");
                        for (int i = 0; i < 6; i++)
                        {
                            if (i > 0 && i % 3 != 0)
                                ImGui::SameLine();
                            ImGui::Checkbox(m_audioNames[i], &game.audioFlags[i]);
                        }
                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }

                ImGui::Separator();

                if (ImGui::Button("Close / Save", ImVec2(120, 0)))
                {
                    SaveDatabase();
                    m_showEditWindow = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0, 0, 1));
                if (ImGui::Button("Delete Game", ImVec2(120, 0)))
                {
                    m_games.erase(m_games.begin() + m_selectedGameIdx);
                    m_selectedGameIdx = -1;
                    SaveDatabase();
                    m_showEditWindow = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopStyleColor();
            }

            if (m_pendingBrowserOpen)
            {
                ImGui::OpenPopup("File Browser");
                m_pendingBrowserOpen = false;
            }

            RenderFileBrowser();

            ImGui::EndPopup();
        }
    }

    void GameLauncher::RenderUI()
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        ImGui::Columns(2, "MainLayout", true);
        ImGui::SetColumnWidth(0, 300);
        RenderGameList();
        ImGui::NextColumn();
        RenderGameDashboard();
        ImGui::Columns(1);

        RenderEditWindow();

        if (m_dreammExePath.empty() && !m_showFileBrowser)
        {
            if (!ImGui::IsPopupOpen("Locate DREAMM"))
                ImGui::OpenPopup("Locate DREAMM");
        }

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Locate DREAMM", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("Welcome! Please locate 'dreamm.exe'.");
            ImGui::Separator();
            ImGui::Text("%s", m_dreammExePath.empty() ? "(None)" : m_dreammExePath.c_str());

            if (ImGui::Button("Browse System..."))
            {
                OpenFileBrowser("Locate DREAMM.exe", &m_dreammExePath, {".exe"});
            }

            if (!m_dreammExePath.empty())
            {
                ImGui::Separator();
                if (ImGui::Button("Save"))
                {
                    SaveDatabase();
                    ImGui::CloseCurrentPopup();
                }
            }

            if (m_pendingBrowserOpen)
            {
                ImGui::OpenPopup("File Browser");
                m_pendingBrowserOpen = false;
            }
            RenderFileBrowser();

            ImGui::EndPopup();
        }
    }

} // namespace Core