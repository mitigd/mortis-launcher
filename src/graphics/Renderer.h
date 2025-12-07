#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

namespace Game { class GameState; }

namespace Graphics {

class Renderer {
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    bool Initialize();
    void Shutdown();
    void BeginFrame(int width, int height);
    void RenderBackground(float time);
    void RenderGameObjects(const Game::GameState& gameState, float time);
    void EndFrame();

private:
    // Background Rendering Resources
    GLuint m_backgroundProg = 0;
    GLuint m_backgroundVAO = 0; 
    GLint m_backgroundTimeLoc = -1;
    GLint m_backgroundResLoc = -1;

    // Game Object Rendering Resources (Shared Shader)
    GLuint m_gameObjectProg = 0;
    GLint m_gameObjectOffsetLoc = -1;
    GLint m_gameObjectColorLoc = -1;

    // --- Static Shader Helpers ---
    static GLuint CompileShader(GLenum type, const char* src);
    static GLuint LinkProgram(const char* vsSrc, const char* fsSrc);
    static GLuint LinkProgramFromShaders(GLuint vs, GLuint fs);
};

} // namespace Graphics

#endif // GRAPHICS_RENDERER_H