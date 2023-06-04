#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader/shader.h"

#include <iostream>
#include <chrono>

constexpr int SCREEN_WIDTH = 640;
constexpr int SCREEN_HEIGHT = 480;

float zoomLevel = 3.0f;
float zoomSpeed = 2.0f;
float mouseX = 0.0f;
float mouseY = 0.0f;
float mouseSensitivity = 1.0f;

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    float zoomPower = std::pow(zoomLevel, 0.5f);
    zoomLevel += yOffset * zoomSpeed * zoomPower;
    zoomLevel = std::max(zoomLevel, 0.1f);
    mouseSensitivity = 1.0f / zoomPower;
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    static double lastX = xPos;
    static double lastY = yPos;

    double deltaX = (xPos - lastX) * mouseSensitivity;
    double deltaY = (yPos - lastY) * mouseSensitivity;

    mouseX += deltaX / (SCREEN_WIDTH * 0.5f);
    mouseY -= deltaY / (SCREEN_HEIGHT * 0.5f);

    lastX = xPos;
    lastY = yPos;
}

int main()
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tee hee", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW initialization failed" << std::endl;
        return -1;
    }

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec4 position;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = position;\n"
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"
        "layout(location = 0) out vec4 color;\n"
        "uniform float time;\n"
        "uniform float zoomLevel;\n"
        "uniform vec2 mousePosition;\n"
        "void main()\n"
        "{\n"
        "    vec2 fragCoord = gl_FragCoord.xy;\n"
        "    vec2 iResolution = vec2(640, 480);\n"
        "    vec2 p = (2.0 * fragCoord - iResolution) / iResolution.y;\n"
        "    p = p / zoomLevel - mousePosition;\n"
        "    vec2 z = vec2(0.0);\n"
        "    vec2 c = p;\n"
        "    int iterations = 500;\n"
        "    int maxIterations = 100;\n"
        "    for (int i = 0; i < iterations; i++) {\n"
        "        if (dot(z, z) > 4.0) {\n"
        "            float t = float(i) / float(maxIterations);\n"
        "            color = vec4(vec3(t), 1.0);\n"
        "            return;\n"
        "        }\n"
        "        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;\n"
        "    }\n"
        "    color = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "}\n";

    unsigned int shaderProgram = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    auto startTime = std::chrono::high_resolution_clock::now();

    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();

        glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
        glUniform1f(glGetUniformLocation(shaderProgram, "zoomLevel"), zoomLevel);
        glUniform2f(glGetUniformLocation(shaderProgram, "mousePosition"), mouseX, mouseY);

        glRecti(-1, -1, 1, 1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
