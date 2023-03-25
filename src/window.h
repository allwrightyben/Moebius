#pragma once
#include <GLFW/glfw3.h>

struct WindowObjects{
    GLFWwindow *window;
    bool framebufferResized;
};


void initGLFWWindow(WindowObjects *wo, int width, int height);