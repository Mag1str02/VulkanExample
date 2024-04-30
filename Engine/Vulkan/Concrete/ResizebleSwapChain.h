#pragma once

#include "Engine/Vulkan/Concrete/SwapChain.h"

namespace Engine::Vulkan::Concrete {

    class ResizebleSwapChain : public Interface::SwapChain, public RefCounted<ResizebleSwapChain> {
    public:
        static Ref<ResizebleSwapChain> Create(Window* window, Ref<Device> device);

        virtual Ref<Task>   CreateAquireImageTask() override;
        virtual Ref<IImage> GetCurrentImage() override;

        virtual VkExtent2D GetExtent() const override;
        virtual VkFormat   GetFormat() const override;

    private:
        ResizebleSwapChain(Window* window, Ref<Device> device);
        void PresentAquire(VkQueue queue, Fence& fence);

    private:
        class PresentAquireTask : public Task, public RefCounted<PresentAquireTask> {
        public:
            PresentAquireTask(Ref<ResizebleSwapChain> swapchain);

            virtual void Run(VkQueue queue) override;

            virtual Ref<const IFence> GetFence() const override;

        private:
            Concrete::Fence         m_AquiredFence;
            Ref<ResizebleSwapChain> m_SwapChain;
        };

    private:
        Window* m_Window;

        Ref<Device>              m_Device;
        Ref<Concrete::SwapChain> m_SwapChain;
        bool                     m_SubOptimalFlag = false;
    };
};  // namespace Engine::Vulkan::Concrete