#pragma once

#include "SwapChain.h"

namespace Engine::Vulkan {

    class ResizebleSwapChain : public ISwapChain, public RefCounted<ResizebleSwapChain> {
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
        class PresentAquireTask : public Task {
        public:
            PresentAquireTask(Ref<ResizebleSwapChain> swapchain);

            virtual void Run(VkQueue queue) override;
            virtual bool IsCompleted() override;

        private:
            Fence                   m_AquiredFence;
            Ref<ResizebleSwapChain> m_SwapChain;
        };

    private:
        Window* m_Window;

        Ref<Device>    m_Device;
        Ref<SwapChain> m_SwapChain;
    };
};  // namespace Engine::Vulkan