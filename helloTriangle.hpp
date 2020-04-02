//
// Created by Krisu on 2020/4/2.
//

#ifndef VULKAN_TEST_HELLOTRIANGLE_HPP
#define VULKAN_TEST_HELLOTRIANGLE_HPP

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }

private:
    void initWindow() {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(1280, 720, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {

    }

    void mainLoop() {

    }

    void cleanUp() {

    }

private:
    GLFWwindow *window;
};

#endif //VULKAN_TEST_HELLOTRIANGLE_HPP
