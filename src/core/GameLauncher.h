#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <future>
#include <string>
#include <vector>

#include "imgui.h"

namespace Core
{
    namespace fs = std::filesystem;

    // Enums
    enum class GamePlatform
    {
        DOS = 0,
        Windows = 1,
        DreammNative = 2
    };

    enum class GameStatus
    {
        Unplayable = 0,
        Playable = 1
    };

    enum class MachineType
    {
        PC = 0,
        Tandy = 1
    };

    // Data Structures
    struct GameEntry
    {
        // Metadata
        std::string name = "New Game";
        std::string description = "";
        GamePlatform platform = GamePlatform::DOS;
        GameStatus status = GameStatus::Unplayable;

        // Paths
        std::string exePath;
        std::string setupPath;
        std::string installPath;
        std::string isoPath;
        std::string rootPathOverride;

        // Window Settings
        bool forceWindowed = false;
        bool forceMaximized = false;
        bool forceFullscreen = false;

        // Hardware Emulation
        int ramKB = 16384;
        int mips = 60;
        MachineType machine = MachineType::PC;
        bool audioFlags[6] = {false, false, false, true, false, false};
        int videoHwIdx = 5;

        // Display Settings
        int width = 800;
        int height = 600;
        int depth = 32;
    };

    struct FileBrowserEntry
    {
        std::string name;
        fs::path fullPath;
        bool isDirectory;
    };

    // Main Class
    class GameLauncher
    {
    public:
        GameLauncher();
        ~GameLauncher();

        // Lifecycle
        void Initialize();
        void RenderUI();

        // Accessors
        bool IsBackgroundEnabled() const { return m_configEnableBackground; }

    private:
        // Persistence & Data
        void LoadConfig();
        void SaveConfig();
        void LoadDatabase();
        void SaveDatabase();
        void ConvertLegacyDatabase();

        // Logic & Operations
        void ScanDreammGames();
        void SortLibrary();
        void LaunchGame(const GameEntry &game, bool runSetup);
        void CreateDreammFile(const GameEntry &game);
        std::string BuildCommand(const GameEntry &game, bool runSetup);
        std::string ResolveDreammGameName(const std::string &folderID, const std::string &versionID, GamePlatform &outPlatform);

        // UI Rendering - Components
        void RenderGameList();
        void RenderGameDashboard();
        void RenderEditWindow();
        void RenderNewGamesModal();
        void RenderConfigModal();
        void RenderAboutModal();
        void RenderFileBrowser();

        // File Browser Helpers
        void OpenFileBrowser(const std::string &title, std::string *targetStringRef, const std::vector<std::string> &allowedExtensions = {});

        // --- Member Variables ---

        // Core Data
        std::string m_dreammExePath;
        std::vector<GameEntry> m_games;
        int m_selectedGameIdx = -1;

        // Persisted Settings
        bool m_configEnableBackground = true;
        bool m_configMouseWarp = true;
        int m_configTheme = 0;

        // UI State - Main
        char m_filterName[256] = "";
        int m_filterPlatform = 0;
        int m_filterStatus = 0;
        int m_autoScrollFrames = 0;
        std::string m_autoScrollTarget;

        // UI State - Modals & Windows
        bool m_showEditWindow = false;
        bool m_showConfigModal = false;
        bool m_showAboutModal = false;
        bool m_showNewGamesModal = false;
        bool m_showFileBrowser = false;
        
        // UI State - Triggers & Flags
        bool m_triggerConfigModal = false;
        bool m_triggerNewGamesModal = false;
        bool m_pendingAboutOpen = false;
        bool m_pendingBrowserOpen = false;
        int m_newGamesCount = 0;

        // File Browser State
        std::string m_fileBrowserTitle;
        std::string *m_fileBrowserTarget = nullptr;
        std::vector<std::string> m_fileBrowserExtensions;
        fs::path m_browserCurrentPath;
        fs::path m_lastGlobalPath;
        std::vector<FileBrowserEntry> m_browserEntries;
        std::future<std::vector<FileBrowserEntry>> m_loadingFuture;

        // Constants
        const char *m_audioNames[6] = {"speaker", "cms", "adlib", "sb16", "mt32", "gmidi"};
        const char *m_videoHwOptions[6] = {"hercules", "cga", "ega", "mcga", "vga", "svga"};
    };

} // namespace Core

#endif // GAMELAUNCHER_H