//
// Created by Krisu on 2020/4/2.
//

#ifndef VULKAN_TEST_HELLOTRIANGLE_HPP
#define VULKAN_TEST_HELLOTRIANGLE_HPP

#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <string>
#include <optional>

#ifdef NDEBUG
#define ENABLE_VALIDATION_LAYERS false
#else
#define ENABLE_VALIDATION_LAYERS true
#endif


// Proxy function that load vkCreateDebugUtilsMessengerEXT
VkResult proxyCreateDebugUtilsMessengerEXT(VkInstance instance,
                                           const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                           const VkAllocationCallbacks *pAllocator,
                                           VkDebugUtilsMessengerEXT *pDebugMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void proxyDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                        VkDebugUtilsMessengerEXT debugUtilsMessengerExt,
                                        const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugUtilsMessengerExt, pAllocator);
    }
}


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
        setupDebugMessenger();
        pickPhysicalDevice();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanUp() {
        if (ENABLE_VALIDATION_LAYERS) {
            proxyDestroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger,
                                               nullptr);
        }
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        if (ENABLE_VALIDATION_LAYERS && !checkValidationLayerSupport()) {
            throw std::runtime_error(
                    "Validation layers requested, but not avalible");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        instanceCreateInfo.enabledExtensionCount = extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (ENABLE_VALIDATION_LAYERS) {
            instanceCreateInfo.enabledLayerCount = validationLayers.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

            // a debugUtilsMessenger will be created automatically
            populateDebugUtilsMessengerCreateInfo(debugCreateInfo);
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
        } else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create instance");
        }
    }

    void setupDebugMessenger() {
        if (!ENABLE_VALIDATION_LAYERS) {
            return;
        }

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugUtilsMessengerCreateInfo(createInfo);

        if (proxyCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                              &debugUtilsMessenger) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger");
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error(
                    "failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto &device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU");
        }
    }

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete() const {
            return graphicsFamily.has_value();
        }
    };

    static bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        return indices.isComplete();
    }


    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamiliesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount,
                                                 nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamiliesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount,
                                                 queueFamilies.data());

        // find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
        for (int i = 0; i < queueFamilies.size() && !indices.isComplete(); i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
        }

        return indices;
    }

    bool checkValidationLayerSupport() {
        // fetch layer information
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> avalibleLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, avalibleLayers.data());

        for (const char *layerName : validationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties : avalibleLayers) {
                if (std::string(layerName) == layerProperties.layerName) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }
        return true;
    }

    static std::vector<const char *> getRequiredExtensions() {
        // glfw extensions (always requested)
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(
                &glfwExtensionCount);
        std::vector<const char *> extensions(glfwExtensions, glfwExtensions +
                                                             glfwExtensionCount);
        // validation debug utils extension (optional)
        if (ENABLE_VALIDATION_LAYERS) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    // cpp binding not working
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData) {

        std::cerr << "Validation layer: " << pCallbackData->pMessage
                  << std::endl;

        return VK_FALSE;
    }

    /* Generate a DebugUtilsMessengerCreateInfo and filled with data */
    static void populateDebugUtilsMessengerCreateInfo(
            VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

private:
    GLFWwindow *window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugUtilsMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    const std::vector<const char *> validationLayers{
            "VK_LAYER_KHRONOS_validation"
    };

    constexpr static const int WIDTH = 1280;
    constexpr static const int HEIGHT = 720;
};

#endif //VULKAN_TEST_HELLOTRIANGLE_HPP
