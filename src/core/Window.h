#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <glad/glad.h>

namespace Core
{

    class Window
    {
    public:
        Window();
        ~Window();

        bool Create(const char *title, int width, int height);
        void Destroy();
        void SwapBuffers() const;
        void GetDrawableSize(int &w, int &h) const;

        SDL_Window *GetSDLWindow() const { return m_sdlWindow; }
        SDL_GLContext GetGLContext() const { return m_glContext; }

    private:
        static SDL_HitTestResult SDLCALL HitTestCallback(SDL_Window *win, const SDL_Point *pt, void *data);

#ifdef _WIN32
        void EnableMaximizeBox(SDL_Window *window);
#endif

        SDL_Window *m_sdlWindow = nullptr;
        SDL_GLContext m_glContext = nullptr;
    };

} // namespace Core

#endif // WINDOW_H