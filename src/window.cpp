#include "window.h"
#include <cstdlib>
#include <cstdio>

void framebufferResizeCallback(GLFWwindow *window, int width, int height){
    WindowObjects *wo = (WindowObjects*)glfwGetWindowUserPointer(window);
    wo->framebufferResized = true;
}

void initGLFWWindow(WindowObjects *wo, int width, int height){
    if(!glfwInit()){
        printf("GLFW Init failed!\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    wo->window = glfwCreateWindow(width, height, "Moebius", nullptr, nullptr);
    if(wo->window == NULL){
        printf("GLFW window creation failed!\n");
        exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(wo->window, &wo);
    glfwSetFramebufferSizeCallback(wo->window, framebufferResizeCallback);
}