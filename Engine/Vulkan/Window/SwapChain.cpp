#include "SwapChain.h"

#include "Surface.h"

#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/RawCommandBuffer.h"
#include "Engine/Vulkan/Renderer/Device.h"
#include "Engine/Vulkan/Renderer/GraphicsQueue.h"

namespace Engine::Vulkan {

    Ref<SwapChain> SwapChain::Create(Ref<Surface> surface, Ref<SwapChain> old_swapchain) {
        return Ref<SwapChain>(new SwapChain(surface, old_swapchain));
    }
    SwapChain::SwapChain(Ref<Surface> surface, Ref<SwapChain> old_swapchain) : m_Surface(surface) {
        PROFILER_SCOPE("Engine::Vulkan::SwapChain::SwapChain");

        m_Surface->UpdateCapabilities();

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface               = m_Surface->Handle();
        createInfo.minImageCount         = m_Surface->GetMinImageCount();
        createInfo.imageFormat           = m_Surface->GetFormat().format;
        createInfo.imageColorSpace       = m_Surface->GetFormat().colorSpace;
        createInfo.imageExtent           = m_Surface->GetExtent();
        createInfo.imageArrayLayers      = 1;
        createInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        createInfo.preTransform          = m_Surface->GetTransform();
        createInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode           = m_Surface->GetPresentMode();
        createInfo.clipped               = VK_TRUE;
        createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices   = nullptr;

        if (old_swapchain) {
            createInfo.oldSwapchain = old_swapchain->Handle();
            old_swapchain->m_Valid  = false;
        }

        VK_CHECK(vkCreateSwapchainKHR(m_Surface->GetDevice()->GetLogicDevice(), &createInfo, nullptr, &m_SwapChain));

        {
            uint32_t             actual_image_count;
            std::vector<VkImage> m_ImageHandles;
            VK_CHECK(vkGetSwapchainImagesKHR(m_Surface->GetDevice()->GetLogicDevice(), m_SwapChain, &actual_image_count, nullptr));
            m_ImageHandles.resize(actual_image_count);
            m_CurrentImage = actual_image_count;

            VK_CHECK(vkGetSwapchainImagesKHR(m_Surface->GetDevice()->GetLogicDevice(), m_SwapChain, &actual_image_count, m_ImageHandles.data()));

            for (uint32_t i = 0; i < actual_image_count; ++i) {
                m_Images.emplace_back(Image(m_Surface.get(), m_SwapChain, m_ImageHandles[i], i));
            }
        }

        {
            auto cmd_pool = CommandPool::Create(m_Surface->GetDevice(), m_Surface->GetDevice()->GetGraphicsQueue()->FamilyIndex());
            auto cmd      = RawCommandBuffer::Create(cmd_pool);
            cmd->Begin();
            for (const auto& image : m_Images) {
                VkImageMemoryBarrier2 barrier{};
                barrier.sType     = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
                barrier.image     = image.m_Image;
                barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseMipLevel   = 0;
                barrier.subresourceRange.levelCount     = VK_REMAINING_MIP_LEVELS;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.layerCount     = VK_REMAINING_ARRAY_LAYERS;
                cmd->AddImageMemoryBarrier(barrier);
            }
            cmd->End();
            VkSubmitInfo info{};
            info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.commandBufferCount = 1;
            info.pCommandBuffers    = &cmd->Handle();

            VK_CHECK(vkQueueSubmit(m_Surface->GetDevice()->GetGraphicsQueue()->Handle(), 1, &info, VK_NULL_HANDLE));
            m_Surface->GetDevice()->GetGraphicsQueue()->WaitIdle();
        }
    }
    SwapChain::~SwapChain() {
        PROFILER_SCOPE("Engine::Vulkan::SwapChain::~SwapChain");
        vkDestroySwapchainKHR(m_Surface->GetDevice()->GetLogicDevice(), m_SwapChain, nullptr);
    }

    const VkSwapchainKHR& SwapChain::Handle() {
        return m_SwapChain;
    }

    VkResult SwapChain::AcquireImage(VkSemaphore signal_semaphore, VkFence signal_fence) {
        uint32_t next_image;
        VkResult res =
            vkAcquireNextImageKHR(m_Surface->GetDevice()->GetLogicDevice(), m_SwapChain, UINT64_MAX, signal_semaphore, signal_fence, &next_image);
        switch (res) {
            case VK_SUCCESS: m_CurrentImage = next_image; break;
            case VK_SUBOPTIMAL_KHR: m_CurrentImage = next_image; break;
            case VK_ERROR_OUT_OF_DATE_KHR: m_CurrentImage = m_Images.size(); break;
            default: DE_ASSERT_FAIL("Failed to acquire swapchain image {}", (int64_t)res);
        }
        return res;
    }
    Ref<Surface> SwapChain::GetSurface() const {
        return m_Surface;
    }

    Ref<SwapChain::Image> SwapChain::GetCurrentImage() {
        DE_ASSERT(m_CurrentImage != m_Images.size(), "No image was aquired yet");
        return Ref<SwapChain::Image>(shared_from_this(), &m_Images[m_CurrentImage]);
    }
    uint32_t SwapChain::GetImageCount() const {
        return m_Images.size();
    }

    uint32_t SwapChain::Image::GetIndex() const {
        return m_Index;
    }

    SwapChain::Image::Image(Image&& other) : Managed::Image(other), m_Index(other.m_Index), m_SwapChainHandle(other.m_SwapChainHandle) {}
    SwapChain::Image::Image(Surface* surface, VkSwapchainKHR swapchain, VkImage handle, uint32_t index) :
        Managed::Image(handle,
                       surface->GetFormat().format,
                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                       surface->GetExtent(),
                       surface->GetDevice().get()),
        m_Index(index),
        m_SwapChainHandle(swapchain) {}

    const VkSwapchainKHR& SwapChain::Image::GetSwapChainHandle() {
        return m_SwapChainHandle;
    }
}  // namespace Engine::Vulkan