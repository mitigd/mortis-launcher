#include "pch.h"
#include "Core/Window.h"

#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../stb_image_resize2.h"

// --- Windows Specific Includes ---
#include <SDL_syswm.h>
#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif
// ---------------------------------

namespace Core
{

    Window::Window() = default;

    Window::~Window()
    {
        Destroy();
    }

#ifdef _WIN32
    static void SetWindowsDarkMode(SDL_Window *window)
    {
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        if (SDL_GetWindowWMInfo(window, &info) && info.subsystem == SDL_SYSWM_WINDOWS)
        {
            HWND hwnd = info.info.win.window;

            BOOL useDarkMode = TRUE;
            HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

            if (FAILED(hr))
            {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "DwmSetWindowAttribute failed to set dark mode.");
            }
        }
    }
#endif

    bool Window::Create(const char *title, int width, int height)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Initializing SDL Video...");
        if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_InitSubSystem Error: %s", SDL_GetError());
            return false;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Creating SDL Window...");

        Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

        m_sdlWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
        if (!m_sdlWindow)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
            return false;
        }

#ifdef _WIN32
        SetWindowsDarkMode(m_sdlWindow);
#endif

        int req_format = STBI_rgb_alpha;
        int iconWidth, iconHeight, orig_format;
        unsigned char *rawData = stbi_load("assets/icon.png", &iconWidth, &iconHeight, &orig_format, req_format);

        if (rawData)
        {
            int targetW = 32;
            int targetH = 32;

            std::vector<unsigned char> resizedData(targetW * targetH * 4);

            stbir_resize_uint8_linear(rawData, iconWidth, iconHeight, 0,
                                      resizedData.data(), targetW, targetH, 0,
                                      STBIR_RGBA);

            Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#endif

            SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(
                resizedData.data(),
                targetW, targetH,
                32,
                4 * targetW,
                rmask, gmask, bmask, amask);

            if (icon)
            {
                SDL_SetWindowIcon(m_sdlWindow, icon);
                SDL_FreeSurface(icon);
            }

            stbi_image_free(rawData);
        }

        m_glContext = SDL_GL_CreateContext(m_sdlWindow);
        if (!m_glContext)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext Error: %s", SDL_GetError());
            return false;
        }

        SDL_GL_MakeCurrent(m_sdlWindow, m_glContext);

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize GLAD");
            return false;
        }

        SDL_GL_SetSwapInterval(1);

        return true;
    }

    void Window::Destroy()
    {
        if (m_glContext)
        {
            SDL_GL_DeleteContext(m_glContext);
            m_glContext = nullptr;
        }
        if (m_sdlWindow)
        {
            SDL_DestroyWindow(m_sdlWindow);
            m_sdlWindow = nullptr;
        }
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    }

    void Window::SwapBuffers() const
    {
        if (m_sdlWindow)
            SDL_GL_SwapWindow(m_sdlWindow);
    }

    void Window::GetDrawableSize(int &w, int &h) const
    {
        if (m_sdlWindow)
            SDL_GL_GetDrawableSize(m_sdlWindow, &w, &h);
        else
        {
            w = 0;
            h = 0;
        }
    }

} // namespace Core