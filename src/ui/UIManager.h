#ifndef UI_UIMANAGER_H
#define UI_UIMANAGER_H

struct SDL_Window;
typedef union SDL_Event SDL_Event;
typedef void* SDL_GLContext;
namespace Game { class GameState; }

#include <imgui.h>

namespace UI {

class UIManager {
public:
    UIManager();
    ~UIManager();

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    UIManager(UIManager&&) = delete;
    UIManager& operator=(UIManager&&) = delete;

    bool Initialize(SDL_Window* window, SDL_GLContext glContext);
    void Shutdown();
    void ProcessEvent(const SDL_Event& event);
    void BeginFrame();
    void Render();
    void EndFrame();

private:

    void RenderTitleBar(SDL_Window* window);
    void RenderContentWindow();
    static void PostQuit();

    int m_titleBarSpacing = 0; 

    ImFont* m_titleFont = nullptr;
};

} // namespace UI

#endif // UI_UIMANAGER_H