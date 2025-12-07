#include "pch.h"

#include "UI/UIManager.h"
#include "Core/GameLauncher.h"

static Core::GameLauncher g_Launcher;
static bool g_LauncherInitialized = false;

constexpr int kTitleBarH = 0;
constexpr int kRightPad = 8;

int g_Spacing = 0;

namespace UI
{

    UIManager::UIManager() = default;

    UIManager::~UIManager()
    {
    }

    bool UIManager::Initialize(SDL_Window *window, SDL_GLContext glContext)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing UIManager (ImGui)...");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        float scale = (float)h / 600.0f;
        if (scale < 1.0f)
            scale = 1.0f;

        SDL_Log("UI Scale Factor: %.2f", scale);

        const char *fontPath = "C:\\Windows\\Fonts\\segoeui.ttf";

        FILE *f = fopen(fontPath, "rb");
        if (f)
        {
            fclose(f);
            io.Fonts->AddFontFromFileTTF(fontPath, 18.0f * scale);

            m_titleFont = io.Fonts->AddFontFromFileTTF(fontPath, 30.0f * scale);
        }
        else
        {
            ImFontConfig config;
            config.SizePixels = 13.0f * scale;
            io.Fonts->AddFontDefault(&config);

            if (io.Fonts->Fonts.Size > 0)
            {
                m_titleFont = io.Fonts->Fonts[0];
            }
        }

        ImGui::StyleColorsDark();

        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;

        style.ScaleAllSizes(scale);

        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);

        g_Spacing = static_cast<int>(style.ItemSpacing.x);
        m_titleBarSpacing = g_Spacing;

        if (!ImGui_ImplSDL2_InitForOpenGL(window, glContext))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ImGui_ImplSDL2_InitForOpenGL failed");
            return false;
        }
        const char *glsl_version = "#version 330 core";
        if (!ImGui_ImplOpenGL3_Init(glsl_version))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ImGui_ImplOpenGL3_Init failed");
            return false;
        }

        return true;
    }

    void UIManager::Shutdown()
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Shutting down UIManager...");
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void UIManager::ProcessEvent(const SDL_Event &event)
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    void UIManager::BeginFrame()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void UIManager::Render()
    {
        RenderContentWindow();
    }

    void UIManager::EndFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // --- Private Render Helpers ---
    void UIManager::RenderTitleBar(SDL_Window *window)
    {
        const ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, (float)kTitleBarH));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.05f, 0.85f));

        ImGui::Begin("###CustomTitleBar", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        // Title text
        ImGui::SetCursorPosX(8.0f);
        ImGui::SetCursorPosY((kTitleBarH - ImGui::GetTextLineHeight()) * 0.5f);
        ImGui::TextUnformatted("Mortis Launcher");

        // System buttons
        const float buttonHeight = kTitleBarH * 0.8f;
        const float buttonWidth = buttonHeight * 1.5f;
        const float buttonsTotalWidth = buttonWidth * 3 + m_titleBarSpacing * 2 + kRightPad;
        ImGui::SameLine(ImGui::GetWindowWidth() - buttonsTotalWidth);

        auto centerButtonY = [&](float height)
        { ImGui::SetCursorPosY((kTitleBarH - height) * 0.5f); };

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

        centerButtonY(buttonHeight);
        if (ImGui::Button("_", ImVec2(buttonWidth, buttonHeight)))
            SDL_MinimizeWindow(window);
        ImGui::SameLine(0.0f, (float)m_titleBarSpacing);

        centerButtonY(buttonHeight);
        bool isMaximised = (SDL_GetWindowFlags(window) & SDL_WINDOW_MAXIMIZED);
        if (ImGui::Button(isMaximised ? "[#]" : "[ ]", ImVec2(buttonWidth, buttonHeight)))
        {
            if (isMaximised)
                SDL_RestoreWindow(window);
            else
                SDL_MaximizeWindow(window);
        }
        ImGui::SameLine(0.0f, (float)m_titleBarSpacing);

        centerButtonY(buttonHeight);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("X", ImVec2(buttonWidth, buttonHeight)))
            PostQuit();
        ImGui::PopStyleColor(2);

        ImGui::PopStyleVar();

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    void UIManager::RenderContentWindow()
    {
        const ImGuiIO &io = ImGui::GetIO();

        if (!g_LauncherInitialized)
        {
            g_Launcher.Initialize();
            g_LauncherInitialized = true;
        }

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

        ImGui::Begin("###ContentWindow", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

        g_Launcher.RenderUI();

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    // --- Static Helpers ---
    void UIManager::PostQuit()
    {
        SDL_Event ev{};
        ev.type = SDL_QUIT;
        SDL_PushEvent(&ev);
    }

} // namespace UI