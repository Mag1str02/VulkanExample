#include "SwapChain.h"

#include <GLFW/glfw3.h>

#include "Vulkan/Device.h"
#include "Vulkan/Helpers.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Renderer.h"
#include "Vulkan/Utils/Assert.h"
#include "Vulkan/Window.h"


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

    SwapChain::SwapChain(Ref<Window> window, Ref<Device> device) {
        DE_ASSERT(window, "Bad window");
        DE_ASSERT(device, "Bad device");

        m_Window = window;
        m_Device = device;

        auto res = glfwCreateWindowSurface(m_Device->GetInstance()->Handle(), m_Window->Handle(), nullptr, &m_Surface);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create window surface");
        m_Details = Helpers::GetSwapChainSupportDetails(m_Device->GetPhysicalDevice(), m_Surface);

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

        uint32_t queueFamilyIndices[] = {*m_Device->GetFamilyIndex(QueueFamily::Graphics), *m_Device->GetFamilyIndex(QueueFamily::Presentation)};
        if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;        // Optional
            createInfo.pQueueFamilyIndices   = nullptr;  // Optional
        }

        VK_CHECK(vkCreateSwapchainKHR(m_Device->GetLogicDevice(), &createInfo, nullptr, &m_SwapChain));

        {
            uint32_t actualImageCount;
            VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicDevice(), m_SwapChain, &actualImageCount, nullptr));
            DE_ASSERT(actualImageCount == imageCount, "Bad image count");
            m_Images.resize(actualImageCount);
            VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicDevice(), m_SwapChain, &actualImageCount, m_Images.data()));
        }

        m_ImageViews.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = m_Images[i];
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = m_Format.format;
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;
            VK_CHECK(vkCreateImageView(m_Device->GetLogicDevice(), &createInfo, nullptr, &m_ImageViews[i]));
        }
    }

    SwapChain::~SwapChain() {
        for (auto imageView : m_ImageViews) {
            vkDestroyImageView(m_Device->GetLogicDevice(), imageView, nullptr);
        }
        vkDestroySwapchainKHR(m_Device->GetLogicDevice(), m_SwapChain, nullptr);
        vkDestroySurfaceKHR(m_Device->GetInstance()->Handle(), m_Surface, nullptr);
    }

    const VkSwapchainKHR& SwapChain::Handle() {
        return m_SwapChain;
    }
    const VkImage& SwapChain::GetImage(uint32_t index) {
        DE_ASSERT(index < m_Images.size(), "Bad index");
        return m_Images[index];
    }
    const VkImageView& SwapChain::GetImageView(uint32_t index) {
        DE_ASSERT(index < m_Images.size(), "Bad index");
        return m_ImageViews[index];
    }

    VkFormat SwapChain::GetFormat() const {
        return m_Format.format;
    }
    VkExtent2D SwapChain::GetExtent() const {
        return m_Extent;
    }
    uint32_t SwapChain::Size() const {
        return m_Images.size();
    }

}  // namespace Vulkan