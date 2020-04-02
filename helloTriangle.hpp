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
#include <string>

class HelloTriangleApplication {
public:
    void Run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }

private:
    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        auto errorFun = [](int error, const char *description) {
            throw std::runtime_error(
                    "GFLW error: " + std::to_string(error) + ": " +
                    description);
        };
        glfwSetErrorCallback(errorFun);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanUp() {
        instance.destroy();

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested, but not avalible");
        }

        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        vk::InstanceCreateInfo createInfo{};
        createInfo.pApplicationInfo = &appInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;

        // extension to glfw
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

        // check how many extensions supported
        uint32_t extensionCount = 0;
        vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                 nullptr,
                                                 VULKAN_HPP_DEFAULT_DISPATCHER);
        std::vector<vk::ExtensionProperties> extensions(extensionCount);
        vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                 extensions.data());

        std::cout << "Avalible extensions:\n";
        for (const auto &extension : extensions) {
            std::cout << "\t" << extension.extensionName << std::endl;
        }

        try {
            vk::createInstance(&createInfo, nullptr, &instance);
        } catch (std::system_error e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vk::enumerateInstanceLayerProperties(&layerCount, nullptr,
                                             VULKAN_HPP_DEFAULT_DISPATCHER);
        std::vector<vk::LayerProperties> avalibleLayers(layerCount);
        vk::enumerateInstanceLayerProperties(&layerCount, avalibleLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : avalibleLayers) {
                if (std::string(layerName) == layerProperties.layerName) {
                    layerFound = true;
                    std::cout << "Found layer: " << layerName <<std::endl;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }
        return true;
    }

private:
    GLFWwindow *window;
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    vk::Instance instance;

    const std::vector<const char *> validationLayers{
            "VK_LAYER_KHRONOS_validation"
    };
#ifdef NDBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
};

#endif //VULKAN_TEST_HELLOTRIANGLE_HPP
