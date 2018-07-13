#include <iostream>

#include <kiryu/screen.h>

Screen::Screen(uint16_t width, uint16_t height):
    m_width(width), m_height(height), m_textureData(nullptr)
{
    m_successInit = true;

    if (!glfwInit()) {
        std::cerr << "GLFW could not init!" << std::endl;
        m_successInit = false;
        return;
    }

    // TODO: Doesn't fully work in i3wm ?
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height,
            "Kiryu", NULL, NULL);
    if (!m_window) {
        m_successInit = false;
        return;
    }

    glfwSetWindowCloseCallback(m_window, destroyWindowOnClose);
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1); // V-sync

    // Vertex array objext
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Vertex buffer object
    glGenBuffers(1, &m_vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(m_verticesAndUv), m_verticesAndUv, GL_STATIC_DRAW);

    // Vertex shader
    m_vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vShader, 1, &m_vShaderText, NULL);
    glCompileShader(m_vShader);
    GLint m_vShaderCompileStatus;
    glGetShaderiv(m_vShader, GL_COMPILE_STATUS, &m_vShaderCompileStatus);
    if (m_vShaderCompileStatus != GL_TRUE) {
        std::cerr << "Vertex shader did not compile!" << std::endl;
        char buffer[512];
        glGetShaderInfoLog(m_vShader, 512, NULL, buffer);
        std::cerr << buffer << std::endl;

        m_successInit = false;
        return;
    }

    // Fragment shader
    m_fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fShader, 1, &m_fShaderText, NULL);
    glCompileShader(m_fShader);
    GLint fragShaderCompileStatus;
    glGetShaderiv(m_fShader, GL_COMPILE_STATUS, &fragShaderCompileStatus);
    if (fragShaderCompileStatus!= GL_TRUE) {
        std::cerr << "Fragment shader did not compile!" << std::endl;

        char buffer[512];
        glGetShaderInfoLog(m_fShader, 512, NULL, buffer);
        std::cerr << buffer << std::endl;

        m_successInit = false;
        return;
    }

    // Program setup
    m_program = glCreateProgram();
    glAttachShader(m_program, m_vShader);
    glAttachShader(m_program, m_fShader);
    glLinkProgram(m_program);
    glUseProgram(m_program);

    // Vertex attribute
    GLint vPosLocation = glGetAttribLocation(m_program, "vPos");
    if (vPosLocation < 0) {
        std::cerr << "Attribute \"vPos\" was not located!" << std::endl;
        m_successInit = false;
        return;
    }
    glVertexAttribPointer(vPosLocation, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), 0);
    glEnableVertexAttribArray(vPosLocation);

    // Texture coordinates attribute
    GLint texcoordLocation = glGetAttribLocation(m_program, "texCoord");
    if (texcoordLocation < 0) {
        std::cerr << "Attribute \"texCoord\" was not located!" << std::endl;
        m_successInit = false;
        return;
    }
    glEnableVertexAttribArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE,
            4*sizeof(float), (void*)(2*sizeof(float)));

    glGenTextures(1, &m_texture);

    glViewport(0, 0, m_width, m_height);
}

void Screen::destroyWindowOnClose(GLFWwindow *window) {
    glfwDestroyWindow(window);
}

void Screen::bindTexture(float *pixels) {
    m_textureData = pixels;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height,
            0, GL_RGB, GL_FLOAT, pixels);

    glUniform1i(glGetUniformLocation(m_program, "tex"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Screen::~Screen() {
    glDeleteTextures(1, &m_texture);
    glDeleteProgram(m_program);
    glDeleteShader(m_fShader);
    glDeleteShader(m_vShader);
    glDeleteBuffers(1, &m_vBuffer);
    glDeleteVertexArrays(1, &m_vao);

    glfwTerminate();
}

bool Screen::wasCreated() {
    return m_successInit;
}


void Screen::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}


bool Screen::isActive() {
    return !glfwWindowShouldClose(m_window);
}

void Screen::renderTextureWhileActive() {
    glfwMakeContextCurrent(m_window);

    while (isActive()) {
        render();

        if (m_texChanged.exchange(false)) {
            bindTexture(m_textureData);
        }
    }
}
