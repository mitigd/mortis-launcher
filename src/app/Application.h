#ifndef APPLICATION_H
#define APPLICATION_H

namespace Core
{
    class Window;
}
namespace Graphics
{
    class Renderer;
}
namespace UI
{
    class UIManager;
}
namespace Game
{
    class GameState;
}

#include "Core/Window.h"
#include "Graphics/Renderer.h"
#include "UI/UIManager.h"
#include "core/GameLauncher.h"

namespace App
{

    class Application
    {
    public:
        Application();
        ~Application();

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;
        Application(Application &&) = delete;
        Application &operator=(Application &&) = delete;

        bool Initialize();
        void Run();
        void Shutdown();

    private:
        void ProcessEvents();
        void Update(float deltaTime);
        void Render();

        Core::Window m_window;
        Graphics::Renderer m_renderer;
        UI::UIManager m_uiManager;

        bool m_isRunning = false;
        uint64_t m_appStartTime = 0;
        uint64_t m_lastFrameTime = 0;

        Core::GameLauncher m_gameLauncher;
    };

} // namespace App

#endif // APPLICATION_H