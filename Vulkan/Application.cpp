#include "Application.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "Debugger.h"
#include "Helpers.h"
#include "Instance.h"
#include "Utils/Assert.h"
#include "Window.h"

namespace {

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
    while (!m_Window->ShouldClose()) {
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

    m_Window = Window::Create();
}
void Application::TerminateGLFW() {
    m_Window = nullptr;
    glfwTerminate();
}

void Application::InitVulkan() {
    m_Renderer = CreateScope<Vulkan::Renderer>();
    m_Window->SetInstance(m_Renderer->GetInstance());

    auto requitredLayers = Vulkan::Helpers::GetRequiredLayers();

    auto queueFamilies = Vulkan::Helpers::GetDeviceQueueFamilies(m_Renderer->GetInstanceHandle(), m_Renderer->GetPhysicalDevice());

    // Create logic device
    {
        vkGetDeviceQueue(m_Renderer->GetLogicDevice(), *queueFamilies.m_GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Renderer->GetLogicDevice(), *queueFamilies.m_PresentFamily, 0, &m_PresentQueue);

        m_GraphicsQueueFamilyIndex = *queueFamilies.m_GraphicsFamily;
    }

    // Setup swapchain
    {
        SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(m_Renderer->GetPhysicalDevice(), m_Window->Surface());
        VkSurfaceFormatKHR      surfaceFormat    = ChooseSwapSurfaceFormat(swapChainSupport.m_SurfaceFormats);
        VkPresentModeKHR        presentMode      = ChooseSwapPresentMode(swapChainSupport.m_PresentationModes);
        VkExtent2D              extent           = ChooseSwapExtent(swapChainSupport.m_Capabilities, m_Window->Handle());

        uint32_t minImageCount = swapChainSupport.m_Capabilities.minImageCount;
        uint32_t maxImageCount = swapChainSupport.m_Capabilities.maxImageCount;
        uint32_t imageCount    = std::clamp(minImageCount + 1, minImageCount, maxImageCount);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = m_Window->Surface();
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

        auto res = vkCreateSwapchainKHR(m_Renderer->GetLogicDevice(), &createInfo, nullptr, &m_SwapChain);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create swap chain");
    }

    // Get swapchain images
    {
        uint32_t imageCount;
        vkGetSwapchainImagesKHR(m_Renderer->GetLogicDevice(), m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Renderer->GetLogicDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());
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

            auto res = vkCreateImageView(m_Renderer->GetLogicDevice(), &createInfo, nullptr, &m_SwapChainImageViews[i]);
            DE_ASSERT(res == VK_SUCCESS, "Failed to create image view");
        }
    }
}
void Application::TerminateVulkan() {
    for (auto imageView : m_SwapChainImageViews) {
        vkDestroyImageView(m_Renderer->GetLogicDevice(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_Renderer->GetLogicDevice(), m_SwapChain, nullptr);
    m_Renderer = nullptr;
}