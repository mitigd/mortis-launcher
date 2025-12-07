#include "pch.h"

#include "app/Application.h"

namespace App
{

    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    bool Application::Initialize()
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing Application...");

        if (!m_window.Create("Mortis Launcher", 1024, 768))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed.");
            return false;
        }
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Window Initialized.");

        if (!m_renderer.Initialize())
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer initialization failed.");
            m_window.Destroy();
            return false;
        }
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Renderer Initialized.");

        if (!m_uiManager.Initialize(m_window.GetSDLWindow(), m_window.GetGLContext()))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UIManager initialization failed.");
            m_renderer.Shutdown();
            m_window.Destroy();
            return false;
        }
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIManager Initialized.");

        m_gameLauncher.Initialize();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameLauncher Initialized.");

        m_isRunning = true;
        m_appStartTime = SDL_GetTicks64();
        m_lastFrameTime = m_appStartTime;

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application Initialization Complete.");
        return true;
    }

    void Application::Run()
    {
        if (!m_isRunning)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Run() called but application is not running.");
            return;
        }

        const int targetFPS = 60;
        const int frameDelay = 1000 / targetFPS; // ~16ms per frame

        while (m_isRunning)
        {
            Uint64 frameStart = SDL_GetTicks64();

            // Calculate delta time
            uint64_t currentTime = SDL_GetTicks64();
            uint64_t deltaTicks = currentTime > m_lastFrameTime ? currentTime - m_lastFrameTime : 0;
            float deltaTime = static_cast<float>(deltaTicks) / 1000.0f;
            m_lastFrameTime = currentTime;

            if (deltaTime > 0.25f)
            {
                deltaTime = 0.25f;
            }

            // Core loop phases
            ProcessEvents();
            Update(deltaTime);
            Render();

            Uint64 frameTime = SDL_GetTicks64() - frameStart;
            if (frameDelay > frameTime)
            {
                SDL_Delay((Uint32)(frameDelay - frameTime));
            }
        }
    }

    void Application::Shutdown()
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Shutting down Application...");
        m_uiManager.Shutdown();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UIManager Shutdown.");
        m_renderer.Shutdown();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Renderer Shutdown.");
        m_window.Destroy();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Window Shutdown.");
        m_isRunning = false;
    }

    void Application::ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            m_uiManager.ProcessEvent(event);

            switch (event.type)
            {
            case SDL_QUIT:
                m_isRunning = false;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
                    event.window.windowID == SDL_GetWindowID(m_window.GetSDLWindow()))
                {
                    m_isRunning = false;
                }
                break;
            }
        }
    }

    void Application::Update(float deltaTime)
    {
    }

    void Application::Render()
    {
        // Calculate Time
        Uint64 currentTick = SDL_GetTicks64();
        float deltaTime = (currentTick - m_lastFrameTime) / 1000.0f;
        m_lastFrameTime = currentTick;
        float appTimeSeconds = (currentTick - m_appStartTime) / 1000.0f;

        int width, height;
        SDL_GetWindowSize(m_window.GetSDLWindow(), &width, &height);
        m_renderer.BeginFrame(width, height);

        // Setup Background
        ImVec4 themeBg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];

        m_renderer.SetClearColor(themeBg.x, themeBg.y, themeBg.z, 1.0f);

        // Draw Shader (Only if Enabled)
        if (m_gameLauncher.IsBackgroundEnabled())
        {
            m_renderer.RenderBackground(appTimeSeconds);
        }

        // Draw UI on top
        m_uiManager.BeginFrame();
        m_uiManager.Render();
        m_uiManager.EndFrame();

        // Swap Buffers
        m_window.SwapBuffers();
    }

} // namespace App