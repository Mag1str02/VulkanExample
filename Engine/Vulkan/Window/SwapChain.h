#pragma once

#include "Engine/Vulkan/Managed/Image.h"

namespace Engine::Vulkan {

    class SwapChain final : public RefCounted<SwapChain> {
    public:
        class Image : public Managed::Image, NonCopyable {
        public:
            Image(Image&& other);

            uint32_t              GetIndex() const;
            const VkSwapchainKHR& GetSwapChainHandle();

        private:
            Image(Surface* surface, VkSwapchainKHR swapchain, VkImage handle, uint32_t index);

        private:
            friend class SwapChain;

            const uint32_t       m_Index;
            const VkSwapchainKHR m_SwapChainHandle;
        };

    public:
        static Ref<SwapChain> Create(Ref<Surface> surface, Ref<SwapChain> old_swapchain = nullptr);
        ~SwapChain();

        const VkSwapchainKHR& Handle();

        VkResult   AcquireImage(VkSemaphore signal_semaphore, VkFence signal_fence);
        Ref<Image> GetCurrentImage();

        Ref<Surface> GetSurface() const;
        uint32_t     GetImageCount() const;

    private:
        SwapChain(Ref<Surface> surface, Ref<SwapChain> old_swapchain);

    private:
        Ref<Surface> m_Surface;

        VkSwapchainKHR     m_SwapChain = VK_NULL_HANDLE;
        std::vector<Image> m_Images;

        uint32_t m_CurrentImage;
        bool     m_Valid = true;
    };
}  // namespace Engine::Vulkan