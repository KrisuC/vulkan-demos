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
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();
    }

private:
    void InitWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        auto errorFun = [](int error, const char *description) {
            throw std::runtime_error(
                    "GFLW error: " + std::to_string(error) + ": " +
                    description);
        };
        glfwSetErrorCallback(errorFun);
        mWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void InitVulkan() {
        CreateInstance();
        SetupDebugMessenger();
        PickPhysicalDevice();
        CreateLogicalDevice();
    }

    void MainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
        }
    }

    void CleanUp() {
        if (ENABLE_VALIDATION_LAYERS) {
            proxyDestroyDebugUtilsMessengerEXT(mInstance, mDebugUtilsMessenger,
                                               nullptr);
        }
        vkDestroyDevice(mDevice, nullptr);
        vkDestroyInstance(mInstance, nullptr);

        glfwDestroyWindow(mWindow);

        glfwTerminate();
    }

    void CreateInstance() {
        if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport()) {
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

        auto extensions = GetRequiredExtensions();
        instanceCreateInfo.enabledExtensionCount = extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (ENABLE_VALIDATION_LAYERS) {
            instanceCreateInfo.enabledLayerCount = validationLayers.size();
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

            // a debugUtilsMessenger will be created automatically
            PopulateDebugUtilsMessengerCreateInfo(debugCreateInfo);
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
        } else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create instance");
        }
    }

    void SetupDebugMessenger() {
        if (!ENABLE_VALIDATION_LAYERS) {
            return;
        }

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugUtilsMessengerCreateInfo(createInfo);

        if (proxyCreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr,
                                              &mDebugUtilsMessenger) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger");
        }
    }

    void PickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error(
                    "failed to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

        for (const auto &device : devices) {
            if (IsDeviceSuitable(device)) {
                mPhysicalDevice = device;
                break;
            }
        }

        if (mPhysicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU");
        }
    }

    void CreateLogicalDevice() {
        QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;

        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        // ignore in nowadays implementation
        deviceCreateInfo.enabledExtensionCount = 0;
        if (ENABLE_VALIDATION_LAYERS) {
            deviceCreateInfo.enabledLayerCount = validationLayers.size();
            deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr,
                           &mDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device");
        }

        vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mQueue);
    }

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete() const {
            return graphicsFamily.has_value();
        }
    };

    static bool IsDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        return indices.isComplete();
    }


    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamiliesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount,
                                                 nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamiliesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount,
                                                 queueFamilies.data());

        // find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
        for (int i = 0;
             i < queueFamilies.size() && !indices.isComplete(); i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
        }

        return indices;
    }

    bool CheckValidationLayerSupport() {
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

    static std::vector<const char *> GetRequiredExtensions() {
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
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData) {

        std::cerr << "Validation layer: " << pCallbackData->pMessage
                  << std::endl;

        return VK_FALSE;
    }

    /* Generate a DebugUtilsMessengerCreateInfo and filled with data */
    static void PopulateDebugUtilsMessengerCreateInfo(
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
        createInfo.pfnUserCallback = DebugCallback;
    }

private:
    GLFWwindow *mWindow;

    VkInstance                 mInstance;
    VkDebugUtilsMessengerEXT   mDebugUtilsMessenger;
    VkPhysicalDevice           mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice                   mDevice;
    VkQueue                    mQueue;

    const std::vector<const char *> validationLayers{
            "VK_LAYER_KHRONOS_validation"
    };

    constexpr static const int WIDTH = 1280;
    constexpr static const int HEIGHT = 720;
};

#endif //VULKAN_TEST_HELLOTRIANGLE_HPP
