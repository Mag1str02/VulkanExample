#pragma once

#include "Engine/Vulkan/Concrete/Fence.h"
#include "Engine/Vulkan/Interface/SwapChain.h"
#include "Engine/Vulkan/Managed/Image.h"

namespace Engine::Vulkan::Concrete {

    class SwapChain final : public Interface::SwapChain, public RefCounted<SwapChain> {
    public:
        static Ref<SwapChain> Create(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size, VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);
        ~SwapChain();

        virtual Ref<Interface::SwapChain::PresentAquireTask> CreateAquireImageTask() override;

        virtual VkFormat   GetFormat() const override;
        virtual VkExtent2D GetExtent() const override;

        VkSwapchainKHR Handle();

    private:
        class PresentAquireTask : public Interface::SwapChain::PresentAquireTask {
        public:
            PresentAquireTask(Ref<SwapChain> swapchain);

            virtual void RecordBarriers(Managed::CommandBuffer& buffer) const override;
            virtual bool RequiresBarriers() const override;
            virtual bool RequiresSemaphore() const override;

            virtual void Run(VkQueue queue, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) override;

            virtual Ref<IImage> GetAquiredImage() const override;

            virtual bool IsCompleted() const override;
            virtual void Wait() const override;

        private:
            Synchronization::ImageTracker      m_PresentImageTracker;
            std::vector<VkImageMemoryBarrier2> m_PresentImageBarriers;

            Concrete::Fence m_Fence;
            Ref<SwapChain>  m_SwapChain;
            Ref<IImage>     m_AquiredImage;
        };

        class Image : public Managed::Image {
        public:
            Image(SwapChain* swapchain, VkImage image);

            virtual bool SemaphoreRequired() const override;
        };

    private:
        SwapChain(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size, VkSwapchainKHR old_swapchain);
        VkResult PresentLatest(VkQueue queue, VkSemaphore wait_semaphore, const Synchronization::ImageTracker& tracker);

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