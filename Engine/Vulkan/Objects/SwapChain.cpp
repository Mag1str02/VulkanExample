#include "SwapChain.h"

#include "Device.h"
#include "Instance.h"

namespace Engine::Vulkan {

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

    Ref<SwapChain> SwapChain::Create(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size) {
        return Ref<SwapChain>(new SwapChain(surface, device, size));
    }

    SwapChain::SwapChain(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size) : m_Surface(surface), m_Device(device) {
        GetSwapChainSupportDetails();

        m_Extent                     = ChooseSwapExtent(m_Details.m_Capabilities, size);
        m_Format                     = ChooseSwapSurfaceFormat(m_Details.m_SurfaceFormats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(m_Details.m_PresentationModes);
        uint32_t         image_count = ChoseCount(m_Details.m_Capabilities);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface               = m_Surface;
        createInfo.minImageCount         = image_count;
        createInfo.imageFormat           = m_Format.format;
        createInfo.imageColorSpace       = m_Format.colorSpace;
        createInfo.imageExtent           = m_Extent;
        createInfo.imageArrayLayers      = 1;
        createInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform          = m_Details.m_Capabilities.currentTransform;
        createInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode           = presentMode;
        createInfo.clipped               = VK_TRUE;
        createInfo.oldSwapchain          = VK_NULL_HANDLE;
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = nullptr;

        VK_CHECK(vkCreateSwapchainKHR(m_Device->GetLogicDevice(), &createInfo, nullptr, &m_SwapChain));

        {
            uint32_t             actual_image_count;
            std::vector<VkImage> m_ImageHandles;
            VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicDevice(), m_SwapChain, &actual_image_count, nullptr));
            DE_ASSERT(actual_image_count == image_count, "Bad image count");
            m_ImageHandles.resize(actual_image_count);
            m_LatestImage = actual_image_count;
            VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicDevice(), m_SwapChain, &actual_image_count, m_ImageHandles.data()));

            m_Images.reserve(actual_image_count);
            for (size_t i = 0; i < actual_image_count; ++i) {
                m_Images.emplace_back(this, m_ImageHandles[i]);
            }
        }
    }

    SwapChain::~SwapChain() {
        vkDestroySwapchainKHR(m_Device->GetLogicDevice(), m_SwapChain, nullptr);
    }

    void SwapChain::GetSwapChainSupportDetails() {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device->GetPhysicalDevice(), m_Surface, &m_Details.m_Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice(), m_Surface, &formatCount, nullptr);
        m_Details.m_SurfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice(), m_Surface, &formatCount, m_Details.m_SurfaceFormats.data());

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice(), m_Surface, &presentModeCount, nullptr);
        m_Details.m_PresentationModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice(), m_Surface, &presentModeCount, m_Details.m_PresentationModes.data());
    }
    VkSwapchainKHR SwapChain::Handle() {
        return m_SwapChain;
    }
    Ref<IImage> SwapChain::AquireNextImage() {
        PresentAquireTask task(std::dynamic_pointer_cast<SwapChain>(shared_from_this()));
        task.Run(m_Device->GetQueue());
        return task.GetAquiredImage();
    }

    VkFormat SwapChain::GetFormat() const {
        return m_Format.format;
    }
    VkExtent2D SwapChain::GetExtent() const {
        return m_Extent;
    }

    SwapChain::Image::Image(SwapChain* swapchain, VkImage image) {
        m_Image      = image;
        m_Extent     = swapchain->m_Extent;
        m_Device     = swapchain->m_Device;
        m_Format     = swapchain->m_Format.format;
        m_UsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    SwapChain::PresentAquireTask::PresentAquireTask(Ref<SwapChain> swapchain) : m_AquiredFence(swapchain->m_Device), m_SwapChain(swapchain) {}

    void SwapChain::PresentAquireTask::Run(Ref<Queue> queue) {
        if (m_SwapChain->m_LatestImage != m_SwapChain->m_Images.size()) {
            VkPresentInfoKHR info   = {};
            info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            info.waitSemaphoreCount = 0;
            info.pWaitSemaphores    = nullptr;
            info.swapchainCount     = 1;
            info.pSwapchains        = &m_SwapChain->m_SwapChain;
            info.pImageIndices      = &m_SwapChain->m_LatestImage;
            VkResult err            = vkQueuePresentKHR(queue->Handle(), &info);
        }
        VK_CHECK(vkAcquireNextImageKHR(m_SwapChain->m_Device->GetLogicDevice(),
                                       m_SwapChain->m_SwapChain,
                                       UINT64_MAX,
                                       VK_NULL_HANDLE,
                                       m_AquiredFence.Handle(),
                                       &m_SwapChain->m_LatestImage));
        m_AquiredImage = Ref<IImage>(m_SwapChain, &m_SwapChain->m_Images[m_SwapChain->m_LatestImage]);
    }

    void SwapChain::PresentAquireTask::Wait() {
        m_AquiredFence.Wait();
    }
    Ref<IImage> SwapChain::PresentAquireTask::GetAquiredImage() {
        Wait();
        return m_AquiredImage;
    }

}  // namespace Engine::Vulkan