#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <kiryu/common.h>

class Screen {
    public:
        Screen(uint16_t width, uint16_t height);

        ~Screen();

        bool wasCreated();

        void bindTexture(float *pixels);

        void render();

        bool isActive();

    private:
        static void destroyWindowOnClose(GLFWwindow *window);

        const GLfloat m_verticesAndUv[16] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f,  -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };

        const GLchar *m_vShaderText = R"glsl(
            #version 330 core
            in vec2 vPos;
            in vec2 texCoord;
            out vec2 uv;
            void main() {
                gl_Position = vec4(vPos, 0.0, 1.0);
                uv = texCoord;
            })glsl";

        const GLchar *m_fShaderText = R"glsl(
            #version 330 core
            in vec2 uv;
            out vec4 outColor;
            uniform sampler2D tex;
            void main()
            {
                outColor = texture(tex, uv);
            })glsl";

        uint16_t m_width, m_height;
        bool m_successInit;
        GLFWwindow *m_window;
        GLuint m_program, m_fShader, m_vShader, m_vBuffer, m_vao, m_texture;
};
