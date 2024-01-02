#include "Application.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "Debugger.h"
#include "Instance.h"
#include "Config.h"
#include "Utils/Assert.h"

namespace {
    static const std::vector<const char*> s_DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    std::vector<const char*> GetRequiredDeviceExtensions() {
        return s_DeviceExtensions;
    }

    std::unordered_set<std::string> FindUnsupportedDeviceExtension(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        auto                            requiredExtensions = GetRequiredDeviceExtensions();
        std::unordered_set<std::string> unsupportedExtensions(requiredExtensions.begin(), requiredExtensions.end());
        for (const auto& extension : availableExtensions) {
            unsupportedExtensions.erase(extension.extensionName);
        }

        return unsupportedExtensions;
    }

    VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        return deviceProperties;
    }

    struct QueueFamilyIndices {
        std::optional<uint32_t> m_GraphicsFamily;
        std::optional<uint32_t> m_PresentFamily;

        std::unordered_set<uint32_t> GetUniqueQueueIndicies() const {
            std::unordered_set<uint32_t> res;
            if (m_GraphicsFamily) {
                res.insert(*m_GraphicsFamily);
            }
            if (m_PresentFamily) {
                res.insert(*m_PresentFamily);
            }
            return res;
        }
    };

    QueueFamilyIndices GetDeviceQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
            const auto& props = queueFamilies[i];
            if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.m_GraphicsFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.m_PresentFamily = i;
            }
        }

        return indices;
    }

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        DE_ASSERT(false, "Failed to find sutable surface format");
        return availableFormats.front();
    }

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR        m_Capabilities;
        std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
        std::vector<VkPresentModeKHR>   m_PresentationModes;
    };

    SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.m_Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        details.m_SurfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.m_SurfaceFormats.data());

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        details.m_PresentationModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.m_PresentationModes.data());

        return details;
    }

    bool CheckDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
        {
            auto deviceProperties = GetDeviceProperties(device);
            if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                return false;
            }
        }
        {
            auto queueFamilies = GetDeviceQueueFamilies(device, surface);
            if (!queueFamilies.m_GraphicsFamily || !queueFamilies.m_PresentFamily) {
                return false;
            }
        }
        {
            auto unsupportedExtensions = FindUnsupportedDeviceExtension(device);
            if (!unsupportedExtensions.empty()) {
                return false;
            }
        }
        {
            auto swapChainSupportDetails = GetSwapChainSupportDetails(device, surface);
            if (swapChainSupportDetails.m_SurfaceFormats.empty()) {
                return false;
            }
            if (swapChainSupportDetails.m_PresentationModes.empty()) {
                return false;
            }
        }

        return true;
    }

}  // namespace

Application::Application() {
    InitGLFW();
    InitVulkan();
    OnStartUp();
}
Application::~Application() {
    OnShutDown();
    TerminateVulkan();
    TerminateGLFW();
}
void Application::Run() {
    OnStartUp();
    while (!glfwWindowShouldClose(m_Window)) {
        Loop();
    }
    OnShutDown();
}

void Application::Loop() {
    glfwPollEvents();
    OnLoop();
}

void Application::InitGLFW() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    DE_ASSERT(m_Window, "Failed to create Vulkan Window");
}
void Application::TerminateGLFW() {
    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void Application::InitVulkan() {
    // Check prerequirements

    auto requitredLayers = Vulkan::Config::GetRequiredLayers();

    Vulkan::Instance::Initialize();
    Vulkan::Debugger::Initialize();

    // Create surface
    {
        auto res = glfwCreateWindowSurface(Vulkan::Instance::Handle(), m_Window, nullptr, &m_Surface);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create window surface");
    }

    // Create device
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(Vulkan::Instance::Handle(), &deviceCount, nullptr);
        DE_ASSERT(deviceCount != 0, "Failed to find any GPU with vulkan support");
        std::vector<VkPhysicalDevice> devices(deviceCount), sutableDevices;
        vkEnumeratePhysicalDevices(Vulkan::Instance::Handle(), &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (CheckDevice(device, m_Surface)) {
                auto props = GetDeviceProperties(device);
                std::cout << "VK: "
                          << "Found sutable GPU: " << props.deviceName << std::endl;
                sutableDevices.push_back(device);
            }
        }
        m_PhysicalDevice = sutableDevices.front();  // TODO: pick best gpu from sutable gpu
        DE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find sutable GPU");
    }
    auto queueFamilies = GetDeviceQueueFamilies(m_PhysicalDevice, m_Surface);

    // Create logic device
    {
        float priority           = 1.0f;
        auto  uniqueQueues       = queueFamilies.GetUniqueQueueIndicies();
        auto  requiredExtensions = GetRequiredDeviceExtensions();

        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;
        for (const auto& queue : uniqueQueues) {
            VkDeviceQueueCreateInfo createInfo{};
            createInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = queue;
            createInfo.queueCount       = 1;
            createInfo.pQueuePriorities = &priority;
            queuesCreateInfo.push_back(createInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos       = queuesCreateInfo.data();
        createInfo.queueCreateInfoCount    = uniqueQueues.size();
        createInfo.pEnabledFeatures        = &deviceFeatures;
        createInfo.enabledLayerCount       = requitredLayers.size();
        createInfo.ppEnabledLayerNames     = requitredLayers.data();
        createInfo.enabledExtensionCount   = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        auto res = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicDevice);
        DE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to create logic device");
        vkGetDeviceQueue(m_LogicDevice, *queueFamilies.m_GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicDevice, *queueFamilies.m_PresentFamily, 0, &m_PresentQueue);

        m_GraphicsQueueFamilyIndex = *queueFamilies.m_GraphicsFamily;
    }

    // Setup swapchain
    {
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(m_PhysicalDevice, m_Surface);
        VkSurfaceFormatKHR      surfaceFormat    = ChooseSwapSurfaceFormat(swapChainSupport.m_SurfaceFormats);
        VkPresentModeKHR        presentMode      = ChooseSwapPresentMode(swapChainSupport.m_PresentationModes);
        VkExtent2D              extent           = ChooseSwapExtent(swapChainSupport.m_Capabilities, m_Window);

        uint32_t minImageCount = swapChainSupport.m_Capabilities.minImageCount;
        uint32_t maxImageCount = swapChainSupport.m_Capabilities.maxImageCount;
        uint32_t imageCount    = std::clamp(minImageCount + 1, minImageCount, maxImageCount);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = m_Surface;
        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = surfaceFormat.format;
        createInfo.imageColorSpace  = surfaceFormat.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform     = swapChainSupport.m_Capabilities.currentTransform;
        createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode      = presentMode;
        createInfo.clipped          = VK_TRUE;
        createInfo.oldSwapchain     = VK_NULL_HANDLE;

        m_SwapChainExtent      = extent;
        m_SwapChainImageFormat = surfaceFormat.format;

        uint32_t queueFamilyIndices[] = {*queueFamilies.m_GraphicsFamily, *queueFamilies.m_PresentFamily};
        if (queueFamilies.m_GraphicsFamily != queueFamilies.m_PresentFamily) {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;        // Optional
            createInfo.pQueueFamilyIndices   = nullptr;  // Optional
        }

        auto res = vkCreateSwapchainKHR(m_LogicDevice, &createInfo, nullptr, &m_SwapChain);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create swap chain");
    }

    // Get swapchain images
    {
        uint32_t imageCount;
        vkGetSwapchainImagesKHR(m_LogicDevice, m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_LogicDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());
    }

    // Get image views
    {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());
        for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = m_SwapChainImages[i];
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = m_SwapChainImageFormat;
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            auto res = vkCreateImageView(m_LogicDevice, &createInfo, nullptr, &m_SwapChainImageViews[i]);
            DE_ASSERT(res == VK_SUCCESS, "Failed to create image view");
        }
    }
}
void Application::TerminateVulkan() {
    for (auto imageView : m_SwapChainImageViews) {
        vkDestroyImageView(m_LogicDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_LogicDevice, m_SwapChain, nullptr);
    vkDestroyDevice(m_LogicDevice, nullptr);
    vkDestroySurfaceKHR(Vulkan::Instance::Handle(), m_Surface, nullptr);

    Vulkan::Debugger::Terminate();
    Vulkan::Instance::Terminate();
}