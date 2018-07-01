#include <iostream>
#include <Eigen/Dense>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using Eigen::MatrixXd;

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480


static float CUSTOM_FRAME[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
static const int CUSTOM_FRAME_ROW = 40;
static const int CUSTOM_FRAME_COL = 40;

void destroyWindowOnClose(GLFWwindow *window) {
    glfwDestroyWindow(window);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glfwSetWindowSize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
        int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "Key pressed" << std::endl;
    }
}

static const GLfloat vertices[] = {
     -1.0f, -1.0f, 0.0f, 0.0f,
     -1.0f, 1.0f, 0.0f, 1.0f,
     1.0f,  -1.0f, 1.0f, 0.0f,
     1.0f, 1.0f, 1.0f, 1.0f,
};

static const char* vertexShaderText =
"#version 330 core\n"
"in vec2 vPos;\n"
"in vec2 texCoord;\n"
"out vec2 uv;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 0.0, 1.0);\n"
"    uv = texCoord;\n"
"}\n";

static const char* fragmentShaderText =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 outColor;\n"
"uniform sampler2D tex;\n"
"void main()\n"
"{\n"
"    outColor = texture(tex, uv) * vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n";

int main() {

    for (int i = 0; i < WINDOW_WIDTH; i++) {
        for (int j = 0; j < WINDOW_HEIGHT; j++) {
            for (int k = 0; k < 3; k++) {
                float value = 0.0f;
                if (j == CUSTOM_FRAME_ROW || i == CUSTOM_FRAME_COL) {
                    value = 1.0f;
                }
                int index = j * (WINDOW_WIDTH * 3) + i * 3 + k;
                CUSTOM_FRAME[index] = value;
            }
        }
    }

    if (!glfwInit()) {
        std::cerr << "GLFW could not init!" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Doesn't work in i3wm ?
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
            "Kiryu", NULL, NULL);
    if (!window) {
        std::cerr << "Could not create GLFWwindow!" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /* Callbacks */
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowCloseCallback(window, destroyWindowOnClose);
    // Force non-resizable for i3wm
    glfwSetWindowSizeCallback(window, windowResizeCallback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-sync

    if(!gladLoadGL()) {
        std::cerr << "GLAD could not load OpenGL!" << std::endl;
        return EXIT_FAILURE;
    }

    GLuint vertexArrayObject, vertexBuffer,
           vertexShader, fragmentShader, program;
    GLint shaderCompileStatus;

    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
            sizeof(vertices), vertices, GL_STATIC_DRAW);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderText, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus != GL_TRUE) {
        std::cerr << "Vertex shader did not compile!" << std::endl;

        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        std::cerr << buffer << std::endl;

        return EXIT_FAILURE;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderText, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus != GL_TRUE) {
        std::cerr << "Fragment shader did not compile!" << std::endl;

        char buffer[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
        std::cerr << buffer << std::endl;

        return EXIT_FAILURE;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);

    GLint vPosLocation = glGetAttribLocation(program, "vPos");
    if (vPosLocation < 0) {
        std::cerr << "Attribute \"vPos\" was not located!" << std::endl;
        return EXIT_FAILURE;
    }
    glVertexAttribPointer(vPosLocation, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), 0);
    glEnableVertexAttribArray(vPosLocation);

    GLint texcoordLocation = glGetAttribLocation(program, "texCoord");
    if (texcoordLocation < 0) {
        std::cerr << "Attribute \"texCoord\" was not located!" << std::endl;
        return EXIT_FAILURE;
    }
    glEnableVertexAttribArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE,
            4*sizeof(float), (void*)(2*sizeof(float)));

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT,
            0, GL_RGB, GL_FLOAT, CUSTOM_FRAME);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteTextures(1, &texture);
    glDeleteProgram(program);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArrayObject);

    glfwTerminate();

    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << m << std::endl;

    return EXIT_SUCCESS;
}
