#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

namespace Game
{
    class GameState;
} // namespace Game

namespace Graphics
{

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
        Renderer(Renderer &&) = delete;
        Renderer &operator=(Renderer &&) = delete;

        bool Initialize();
        void Shutdown();
        void BeginFrame(int width, int height);
        void RenderBackground(float time);
        void RenderGameObjects(const Game::GameState &gameState, float time);
        void EndFrame();

        void SetClearColor(float r, float g, float b, float a)
        {
            m_clearColor[0] = r;
            m_clearColor[1] = g;
            m_clearColor[2] = b;
            m_clearColor[3] = a;
        }

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
        static GLuint CompileShader(GLenum type, const char *src);
        static GLuint LinkProgram(const char *vsSrc, const char *fsSrc);
        static GLuint LinkProgramFromShaders(GLuint vs, GLuint fs);

        float m_clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    };

} // namespace Graphics

#endif // GRAPHICS_RENDERER_H