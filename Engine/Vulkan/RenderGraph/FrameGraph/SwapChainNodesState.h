#pragma once

#include "Engine/Vulkan/Common.h"

#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan::RenderGraph {

    static constexpr const char* kAquireImageName  = "SwapChainAquireImage";
    static constexpr const char* kPresentImageName = "SwapChainPresentImage";

    class SwapChainNodesState {
    public:
        class Iteration {
        public:
            ~Iteration();

            bool AquireNextImage(VkSemaphore signal_semaphore);
            void SetCurrentPresentSemaphore(Ref<Semaphore> semaphore);

            bool                  IsOutOfDate() const;
            Ref<SwapChain::Image> GetCurrentImage() const;
            VkSwapchainKHR        GetSwapChainHandle() const;

        private:
            Iteration(Ref<SwapChain> swapchain);

        private:
            friend class SwapChainNodesState;

            std::vector<Ref<Semaphore>> m_ImagePresentSemaphores;
            Ref<SwapChain>              m_SwapChain;
            bool                        m_OutOfDate = false;
        };

        SwapChainNodesState(Ref<Surface> surface);

        Ref<Iteration> GetCurrentIteration();
        void           CreateNewIteration();

    private:
        Ref<Iteration> m_Iteration;
        Ref<Surface>   m_Surface;
    };

}  // namespace Engine::Vulkan::RenderGraph