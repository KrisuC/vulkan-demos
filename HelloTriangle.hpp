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
#include <set>

#ifdef NDEBUG
#define ENABLE_VALIDATION_LAYERS false
#else
#define ENABLE_VALIDATION_LAYERS true
#endif


// Proxy function that load vkCreateDebugUtilsMessengerEXT
VkResult proxyCreateDebugUtilsMessengerEXT(VkInstance instance,
                                           const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                           const VkAllocationCallbacks *pAllocator,
                                           VkDebugUtilsMessengerEXT *pDebugMessenger);


void proxyDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                        VkDebugUtilsMessengerEXT debugUtilsMessengerExt,
                                        const VkAllocationCallbacks *pAllocator);

std::vector<char> ReadFile(const std::string& filename);

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};


struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


class HelloTriangleApplication {
public:
    void Run() {
        InitWindow();
        InitVulkan();
        MainLoop();
        CleanUp();
    }

private:
    void InitWindow();

    void InitVulkan() {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateGraphicsPipeline();
    }

    void MainLoop() {
        while (!glfwWindowShouldClose(mWindow)) {
            glfwPollEvents();
        }
    }

    void CleanUp();

    void CreateInstance();

    void SetupDebugMessenger();

    void CreateSurface();

    void PickPhysicalDevice();

    void CreateLogicalDevice();

    void CreateSwapChain();

    void CreateImageViews();

    void CreateGraphicsPipeline();

    VkShaderModule CreateShaderModule(const std::vector<char> &code);

    bool IsDeviceSuitable(VkPhysicalDevice physicalDevice);

    bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);


    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);

    bool CheckValidationLayerSupport();

    SwapChainSupportDetails
    QuerySwapChainSupport(VkPhysicalDevice physicalDevice);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR ChooseSwapPresentMode(
            const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    static std::vector<const char *> GetRequiredExtensions();

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData);

    /* Generate a DebugUtilsMessengerCreateInfo and filled with data */
    static void PopulateDebugUtilsMessengerCreateInfo(
            VkDebugUtilsMessengerCreateInfoEXT &createInfo);

private:
    GLFWwindow *mWindow;

    VkInstance               mInstance;
    VkDebugUtilsMessengerEXT mDebugUtilsMessenger;
    VkPhysicalDevice         mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice                 mDevice;
    VkQueue                  mGraphicsQueue;
    VkQueue                  mPresentQueue;
    VkSurfaceKHR             mSurface;
    VkSwapchainKHR           mSwapChain;
    std::vector<VkImage>     mSwapChainImages;
    VkFormat                 mSwapChainImageFormat;
    VkExtent2D               mSwapChainExtent;
    std::vector<VkImageView> mSwapChainImageViews;

    const std::vector<const char *> mValidationLayers{
            "VK_LAYER_KHRONOS_validation"
    };
    const std::vector<const char *> mDeviceExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    constexpr static const int WIDTH = 1280;
    constexpr static const int HEIGHT = 720;
};

#endif //VULKAN_TEST_HELLOTRIANGLE_HPP
