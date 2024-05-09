#include "SwapChain.h"

#include "Fence.h"

#include "Engine/Vulkan/Device.h"
#include "Engine/Vulkan/Managed/CommandBuffer.h"

namespace Engine::Vulkan::Concrete {

    namespace {
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        m_Capabilities;
            std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
            std::vector<VkPresentModeKHR>   m_PresentationModes;
        };

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
            if (maxImageCount == 0) {
                maxImageCount = UINT32_MAX;
            }
            return std::clamp(minImageCount + 2, minImageCount, maxImageCount);
        }

        SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
            SwapChainSupportDetails details;
            VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.m_Capabilities));

            uint32_t formatCount;
            VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));
            details.m_SurfaceFormats.resize(formatCount);
            VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.m_SurfaceFormats.data()));

            uint32_t presentModeCount;
            VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));
            details.m_PresentationModes.resize(presentModeCount);
            VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.m_PresentationModes.data()));
            return details;
        }

    }  // namespace

    Ref<SwapChain> SwapChain::Create(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size, VkSwapchainKHR old_swapchain) {
        return Ref<SwapChain>(new SwapChain(surface, device, size, old_swapchain));
    }
    SwapChain::SwapChain(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size, VkSwapchainKHR old_swapchain) :
        m_Surface(surface), m_Device(device) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SwapChain::SwapChain");

        auto details = GetSwapChainSupportDetails(m_Device->GetPhysicalDevice(), m_Surface);

        m_Extent                     = ChooseSwapExtent(details.m_Capabilities, size);
        m_Format                     = ChooseSwapSurfaceFormat(details.m_SurfaceFormats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(details.m_PresentationModes);
        uint32_t         image_count = ChoseCount(details.m_Capabilities);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface               = m_Surface;
        createInfo.minImageCount         = image_count;
        createInfo.imageFormat           = m_Format.format;
        createInfo.imageColorSpace       = m_Format.colorSpace;
        createInfo.imageExtent           = m_Extent;
        createInfo.imageArrayLayers      = 1;
        createInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.preTransform          = details.m_Capabilities.currentTransform;
        createInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode           = presentMode;
        createInfo.clipped               = VK_TRUE;
        createInfo.oldSwapchain          = old_swapchain;
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = nullptr;

        VK_CHECK(vkCreateSwapchainKHR(m_Device->GetLogicDevice(), &createInfo, nullptr, &m_SwapChain));

        {
            uint32_t             actual_image_count;
            std::vector<VkImage> m_ImageHandles;
            VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicDevice(), m_SwapChain, &actual_image_count, nullptr));
            m_ImageHandles.resize(actual_image_count);
            m_LatestImage = actual_image_count;

            std::println("Image amount {}", actual_image_count);
            VK_CHECK(vkGetSwapchainImagesKHR(m_Device->GetLogicDevice(), m_SwapChain, &actual_image_count, m_ImageHandles.data()));

            for (size_t i = 0; i < actual_image_count; ++i) {
                m_Images.emplace_back(this, m_ImageHandles[i]);
            }
        }
    }
    SwapChain::~SwapChain() {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SwapChain::~SwapChain");
        vkDestroySwapchainKHR(m_Device->GetLogicDevice(), m_SwapChain, nullptr);
    }

    Ref<Interface::SwapChain::PresentAquireTask> SwapChain::CreateAquireImageTask() {
        return Ref<PresentAquireTask>(new PresentAquireTask(shared_from_this()));
    }

    VkFormat SwapChain::GetFormat() const {
        return m_Format.format;
    }
    VkExtent2D SwapChain::GetExtent() const {
        return m_Extent;
    }

    VkSwapchainKHR SwapChain::Handle() {
        return m_SwapChain;
    }

    SwapChain::Image::Image(SwapChain* swapchain, VkImage image) {
        m_Image      = image;
        m_Extent     = swapchain->m_Extent;
        m_Device     = swapchain->m_Device;
        m_Format     = swapchain->m_Format.format;
        m_UsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    bool SwapChain::Image::SemaphoreRequired() const {
        return true;
    }

    VkResult SwapChain::PresentLatest(VkQueue queue, VkSemaphore wait_semaphore, const Synchronization::ImageTracker& tracker) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SwapChain::PresentLatest");

        if (m_LatestImage != m_Images.size()) {
            DE_ASSERT(wait_semaphore != VK_NULL_HANDLE, "Present must wait on semaphore");

            Synchronization::UpdateImageState(m_Images[m_LatestImage], tracker.GetPrefix(), tracker.GetSuffix());

            VkPresentInfoKHR info   = {};
            info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            info.waitSemaphoreCount = 1;
            info.pWaitSemaphores    = &wait_semaphore;
            info.swapchainCount     = 1;
            info.pSwapchains        = &m_SwapChain;
            info.pImageIndices      = &m_LatestImage;

            std::println("Presenting: {}", m_LatestImage);
            auto res      = vkQueuePresentKHR(queue, &info);
            m_LatestImage = m_Images.size();

            return res;
        }
        return VK_SUCCESS;
    }

    SwapChain::PresentAquireTask::PresentAquireTask(Ref<SwapChain> swapchain) : m_Fence(swapchain->m_Device), m_SwapChain(swapchain) {
        auto latest_image = m_SwapChain->m_LatestImage;
        if (latest_image != m_SwapChain->m_Images.size()) {
            auto& image = m_SwapChain->m_Images[latest_image];

            m_PresentImageTracker.AccessRequest(Synchronization::AccessScope(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            m_PresentImageBarriers = Synchronization::ConnectSyncStates(image, image.GetSyncState(), m_PresentImageTracker.GetPrefix());
        }
    }

    void SwapChain::PresentAquireTask::Run(VkQueue queue, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SwapChain::PresentAquireTask::Run");

        VK_CHECK(m_SwapChain->PresentLatest(queue, wait_semaphore, m_PresentImageTracker));
        VK_CHECK(vkAcquireNextImageKHR(m_SwapChain->m_Device->GetLogicDevice(),
                                       m_SwapChain->m_SwapChain,
                                       UINT64_MAX,
                                       signal_semaphore,
                                       m_Fence.Handle(),
                                       &m_SwapChain->m_LatestImage));
        m_AquiredImage = Ref<IImage>(m_SwapChain, &m_SwapChain->m_Images[m_SwapChain->m_LatestImage]);
    }

    void SwapChain::PresentAquireTask::RecordBarriers(Managed::CommandBuffer& buffer) const {
        for (const auto& barrier : m_PresentImageBarriers) {
            buffer.AddImageMemoryBarrier(barrier);
        }
    }
    bool SwapChain::PresentAquireTask::RequiresBarriers() const {
        return !m_PresentImageBarriers.empty();
    }
    bool SwapChain::PresentAquireTask::RequiresSemaphore() const {
        return true;
    }

    Ref<IImage> SwapChain::PresentAquireTask::GetAquiredImage() const {
        DE_ASSERT(m_AquiredImage != nullptr, "PresentAquireTask was not runned yet");
        return m_AquiredImage;
    }

    bool SwapChain::PresentAquireTask::IsCompleted() const {
        return m_Fence.IsSignaled();
    }
    void SwapChain::PresentAquireTask::Wait() const {
        m_Fence.Wait();
    }

}  // namespace Engine::Vulkan::Concrete