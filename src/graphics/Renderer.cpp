#include "pch.h"
#include "Graphics/Renderer.h"

// --- Error Helper Macro ---
#define GL_CHECK(stmt) do { \
    stmt; \
    GLenum err = glGetError(); \
    if (err != GL_NO_ERROR) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL Error 0x%x at %s:%d: %s", err, __FILE__, __LINE__, #stmt); \
} while (0)

namespace Graphics
{
    Renderer::Renderer() = default;

    Renderer::~Renderer() {
        if (m_backgroundProg != 0) Shutdown();
    }

    bool Renderer::Initialize()
    {
        // Fullscreen Triangle Vertex Shader
        const char* vsSrc = R"(#version 330 core
        const vec2 v[3] = vec2[3](vec2(-1,-1), vec2(3,-1), vec2(-1,3));
        out vec2 uv;
        void main() {
            gl_Position = vec4(v[gl_VertexID], 0.0, 1.0);
            uv = gl_Position.xy * 0.5 + 0.5;
        })";

        // PS3-Style Fragment Shader
        const char* fsSrc = R"(#version 330 core
        in vec2 uv;
        out vec4 frag;
        uniform float iTime;
        uniform vec2 iResolution;

        void main() {
            vec2 aspect_uv = uv * vec2(iResolution.x / iResolution.y, 1.0);
            float t = iTime * 0.15;
            float pattern = 0.0;
            float y_influence = sin(uv.y * 1.5 + t * 0.5) * 0.5 + 0.5;

            pattern += sin(uv.y * 5.0 + t + aspect_uv.x * (0.5 + y_influence)) * 0.5;
            pattern += cos(uv.y * 3.0 - t * 1.1 + aspect_uv.x * 0.8) * 0.4;

            float v = smoothstep(-1.2, 1.2, pattern);
            v = v * v * (3.0 - 2.0 * v); // Cubic smoothstep approx

            vec3 c = mix(vec3(0.05, 0.05, 0.10), vec3(0.15, 0.15, 0.25), smoothstep(0.0, 0.4, v));
            c = mix(c, vec3(0.30, 0.30, 0.35), smoothstep(0.3, 0.7, v));
            c = mix(c, vec3(0.45, 0.45, 0.45), smoothstep(0.65, 0.9, v));

            frag = vec4(clamp(c, 0.0, 1.0), 1.0);
        })";

        // Compile & Link
        m_backgroundProg = LinkProgram(vsSrc, fsSrc);
        if (!m_backgroundProg) return false;

        // Get Uniforms & Create Dummy VAO (Required for Core Profile)
        m_backgroundTimeLoc = glGetUniformLocation(m_backgroundProg, "iTime");
        m_backgroundResLoc = glGetUniformLocation(m_backgroundProg, "iResolution");
        GL_CHECK(glGenVertexArrays(1, &m_backgroundVAO));

        return true;
    }

    void Renderer::Shutdown()
    {
        if (m_backgroundProg) { GL_CHECK(glDeleteProgram(m_backgroundProg)); m_backgroundProg = 0; }
        if (m_backgroundVAO)  { GL_CHECK(glDeleteVertexArrays(1, &m_backgroundVAO)); m_backgroundVAO = 0; }
    }

    void Renderer::BeginFrame(int width, int height)
    {
        GL_CHECK(glViewport(0, 0, width, height));
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::RenderBackground(float time)
    {
        if (!m_backgroundProg) return;

        int w, h;
        SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &w, &h);

        GL_CHECK(glUseProgram(m_backgroundProg));
        if (m_backgroundTimeLoc != -1) glUniform1f(m_backgroundTimeLoc, time);
        if (m_backgroundResLoc != -1)  glUniform2f(m_backgroundResLoc, (float)w, (float)h);

        GL_CHECK(glBindVertexArray(m_backgroundVAO));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
        GL_CHECK(glBindVertexArray(0));
    }

    // --- Helpers ---

    GLuint Renderer::CompileShader(GLenum type, const char* src)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512]; glGetShaderInfoLog(shader, 512, nullptr, log);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Shader Error: %s", log);
            return 0;
        }
        return shader;
    }

    GLuint Renderer::LinkProgram(const char* vsSrc, const char* fsSrc)
    {
        GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSrc);
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSrc);
        if (!vs || !fs) return 0;

        GLuint prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);

        glDeleteShader(vs);
        glDeleteShader(fs);

        GLint success;
        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512]; glGetProgramInfoLog(prog, 512, nullptr, log);
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Link Error: %s", log);
            return 0;
        }
        return prog;
    }

} // namespace Graphics