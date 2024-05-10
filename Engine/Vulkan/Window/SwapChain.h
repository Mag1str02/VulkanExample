#pragma once

#include "Engine/Vulkan/Managed/Image.h"

namespace Engine::Vulkan {

    class SwapChain final : public RefCounted<SwapChain> {
    public:
        static Ref<SwapChain> Create(Ref<Surface> surface, Ref<SwapChain> old_swapchain = nullptr);
        ~SwapChain();

        const VkSwapchainKHR& Handle();

        std::pair<VkResult, Ref<IImage>> AcquireImage(VkSemaphore signal_semaphore, VkFence signal_fence);

        uint32_t GetCurrentImageIndex() const;
        uint32_t GetImageCount() const;

    private:
        SwapChain(Ref<Surface> surface, Ref<SwapChain> old_swapchain);

    private:
        Ref<Surface> m_Surface;

        VkSwapchainKHR              m_SwapChain = VK_NULL_HANDLE;
        std::vector<Managed::Image> m_Images;

        uint32_t m_CurrentImage;
        bool     m_Valid = true;
    };
}  // namespace Engine::Vulkan