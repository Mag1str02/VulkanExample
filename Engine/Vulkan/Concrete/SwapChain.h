#pragma once

#include "Engine/Vulkan/Fence.h"
#include "Engine/Vulkan/Interface/SwapChain.h"
#include "Engine/Vulkan/Managed/Image.h"

namespace Engine::Vulkan::Concrete {

    class SwapChain : public Interface::SwapChain, public RefCounted<SwapChain> {
    public:
        static Ref<SwapChain> Create(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size, VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);
        ~SwapChain();

        virtual Ref<Task>   CreateAquireImageTask() override;
        virtual Ref<IImage> GetCurrentImage() override;

        virtual VkFormat   GetFormat() const override;
        virtual VkExtent2D GetExtent() const override;

        VkSwapchainKHR Handle();

    private:
        SwapChain(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size, VkSwapchainKHR old_swapchain);
        void PresentLatest(VkQueue queue);

    private:
        class PresentAquireTask : public Task {
        public:
            PresentAquireTask(Ref<SwapChain> swapchain);

            virtual void Run(VkQueue queue) override;
            virtual bool IsCompleted() override;

        private:
            ManualLifetime<Fence> m_AquiredFence;
            Ref<SwapChain>        m_SwapChain;
        };

        class Image : public Managed::Image {
        public:
            Image(SwapChain* swapchain, VkImage image);
        };

    private:
        friend class ResizebleSwapChain;

        VkExtent2D         m_Extent = {};
        VkSurfaceFormatKHR m_Format = {};

        Ref<Device>        m_Device;
        VkSurfaceKHR       m_Surface   = VK_NULL_HANDLE;
        VkSwapchainKHR     m_SwapChain = VK_NULL_HANDLE;
        std::vector<Image> m_Images;

        uint32_t m_LatestImage;
    };
}  // namespace Engine::Vulkan::Concrete