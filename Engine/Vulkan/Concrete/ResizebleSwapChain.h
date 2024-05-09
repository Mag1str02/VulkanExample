#pragma once

#include "Engine/Vulkan/Concrete/SwapChain.h"

namespace Engine::Vulkan::Concrete {

    class ResizebleSwapChain : public Interface::SwapChain, public RefCounted<ResizebleSwapChain> {
    public:
        static Ref<ResizebleSwapChain> Create(Window* window, Ref<Device> device);

        virtual Ref<Interface::SwapChain::PresentAquireTask> CreateAquireImageTask() override;

        virtual VkExtent2D GetExtent() const override;
        virtual VkFormat   GetFormat() const override;

    private:
        ResizebleSwapChain(Window* window, Ref<Device> device);

    private:
        class PresentAquireTask : public Interface::SwapChain::PresentAquireTask {
        public:
            PresentAquireTask(Ref<ResizebleSwapChain> swapchain);

            virtual void RecordBarriers(Managed::CommandBuffer& buffer) const override;
            virtual bool RequiresBarriers() const override;
            virtual bool RequiresSemaphore() const override;

            virtual void Run(VkQueue queue, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) override;

            virtual Ref<IImage> GetAquiredImage() const override;

            virtual bool IsCompleted() const override;
            virtual void Wait() const override;

        private:
            void RecreateSwapChain();

        private:
            Synchronization::ImageTracker      m_PresentImageTracker;
            std::vector<VkImageMemoryBarrier2> m_PresentImageBarriers;

            Concrete::Fence         m_Fence;
            Ref<ResizebleSwapChain> m_SwapChain;
            Ref<IImage>             m_AquiredImage;
        };

    private:
        Window* m_Window;

        Ref<Device>              m_Device;
        Ref<Concrete::SwapChain> m_SwapChain;
        bool                     m_SubOptimalFlag = false;
    };
};  // namespace Engine::Vulkan::Concrete