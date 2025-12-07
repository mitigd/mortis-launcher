#include "pch.h"
#include "App/Application.h"

// Aurebesh - Dream Launcher

#if defined(_WIN32) && !defined(DEBUG) && !defined(_DEBUG)
    #include <windows.h>
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
    int main(int /*argc*/, char* /*argv*/[])
#endif
{
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    App::Application app; 

    try {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Application starting...");

        if (!app.Initialize()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Application initialization failed!");
            app.Shutdown();
            return 1;
        }

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initialization successful. Running main loop...");
        app.Run();

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Main loop finished. Shutting down...");
        app.Shutdown();

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Shutdown complete. Exiting.");
        return 0; 

    } catch (const std::exception& e) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Unhandled exception caught: %s", e.what());
        app.Shutdown();
        return 1;
    } catch (...) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Unknown unhandled exception caught!");
        app.Shutdown();
        return 1;
    }
}