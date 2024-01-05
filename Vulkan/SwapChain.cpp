#include "SwapChain.h"

#include <GLFW/glfw3.h>

#include "Device.h"
#include "Renderer.h"
#include "Vulkan/Utils/Assert.h"
#include "Window.h"
#include "vulkan/vulkan_core.h"

namespace Vulkan {

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

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D desiredExtent) {
            if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
                return capabilities.currentExtent;
            } else {
                desiredExtent.width  = std::clamp(desiredExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                desiredExtent.height = std::clamp(desiredExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
                return desiredExtent;
            }
        }

        uint32_t ChoseCount(const VkSurfaceCapabilitiesKHR& capabilities) {
            uint32_t minImageCount = capabilities.minImageCount;
            uint32_t maxImageCount = capabilities.maxImageCount;
            return std::clamp(minImageCount + 1, minImageCount, maxImageCount);
        }
    }  // namespace

    Ref<SwapChain> SwapChain::Create(Ref<Window> window, Ref<Renderer> renderer) {
        return Ref<SwapChain>(new SwapChain(window, renderer));
    }

    SwapChain::SwapChain(Ref<Window> window, Ref<Renderer> renderer) {
        DE_ASSERT(window, "Bad window");
        DE_ASSERT(renderer, "Bad renderer");

        m_Window   = window;
        m_Renderer = renderer;

        auto res = glfwCreateWindowSurface(m_Renderer->GetInstanceHandle(), m_Window->Handle(), nullptr, &m_Surface);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create window surface");
        m_Details = Helpers::GetSwapChainSupportDetails(m_Renderer->GetPhysicalDevice(), m_Surface);

        int width, height;
        glfwGetFramebufferSize(m_Window->Handle(), &width, &height);
        VkExtent2D initialSize;
        initialSize.width  = width;
        initialSize.height = height;
        ChangeExtent(initialSize);

        m_Format = ChooseSwapSurfaceFormat(m_Details.m_SurfaceFormats);

        Rebuild();
    }

    void SwapChain::Resize(uint32_t width, uint32_t height) {
        VkExtent2D newSize;
        newSize.width  = width;
        newSize.height = height;
        if (ChangeExtent(newSize)) {
            Rebuild();
        }
    }

    bool SwapChain::ChangeExtent(VkExtent2D extent) {
        auto newExtent = ChooseSwapExtent(m_Details.m_Capabilities, extent);
        bool changed   = (newExtent.width != m_Extent.width || newExtent.height != m_Extent.height);
        m_Extent       = newExtent;
        return changed;
    }

    void SwapChain::Rebuild() {
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(m_Details.m_PresentationModes);

        uint32_t imageCount = ChoseCount(m_Details.m_Capabilities);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = m_Surface;
        createInfo.minImageCount    = imageCount;
        createInfo.imageFormat      = m_Format.format;
        createInfo.imageColorSpace  = m_Format.colorSpace;
        createInfo.imageExtent      = m_Extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform     = m_Details.m_Capabilities.currentTransform;
        createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode      = presentMode;
        createInfo.clipped          = VK_TRUE;
        createInfo.oldSwapchain     = VK_NULL_HANDLE;

        const auto& device               = *m_Renderer->GetDevice();
        uint32_t    queueFamilyIndices[] = {*device.GetFamilyIndex(Queue::Family::Graphics), *device.GetFamilyIndex(Queue::Family::Presentation)};
        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
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

        uint32_t actualImageCount;
        vkGetSwapchainImagesKHR(m_Renderer->GetLogicDevice(), m_SwapChain, &actualImageCount, nullptr);
        DE_ASSERT(actualImageCount == imageCount, "Bad image count");
        m_Images.resize(actualImageCount);
        vkGetSwapchainImagesKHR(m_Renderer->GetLogicDevice(), m_SwapChain, &actualImageCount, m_Images.data());
    }

    SwapChain::~SwapChain() {
        vkDestroySwapchainKHR(m_Renderer->GetLogicDevice(), m_SwapChain, nullptr);
        vkDestroySurfaceKHR(m_Renderer->GetInstanceHandle(), m_Surface, nullptr);
    }

    VkSwapchainKHR SwapChain::Handle() {
        return m_SwapChain;
    }
    VkImage SwapChain::GetImage(uint32_t index) {
        DE_ASSERT(index < m_Images.size(), "Bad index");
        return m_Images[index];
    }

    VkFormat SwapChain::GetFormat() const {
        return m_Format.format;
    }
    VkExtent2D SwapChain::GetExtent() const {
        return m_Extent;
    }
    uint32_t SwapChain::GetImageCount() const {
        return m_Images.size();
    }

}  // namespace Vulkan