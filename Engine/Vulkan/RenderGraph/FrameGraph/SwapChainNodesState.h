#pragma once

#include "Engine/Vulkan/Common.h"

#include "Engine/Vulkan/FencePool.h"
#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan::RenderGraph {

    static constexpr const char* kAquireImageName  = "SwapChainAquireImage";
    static constexpr const char* kPresentImageName = "SwapChainPresentImage";

    class SwapChainNodesState {
    public:
        class Iteration {
        public:
            ~Iteration();

            bool AquireNextImage(VkSemaphore signal_semaphore, VkFence fence);
            void SetCurrentPresentSemaphore(Ref<IBinarySemaphore> semaphore);
            void SetCurrentAquireFence(Ref<IFence> fence);

            bool                  IsOutOfDate() const;
            Ref<SwapChain::Image> GetCurrentImage() const;
            VkSwapchainKHR        GetSwapChainHandle() const;

        private:
            Iteration(Ref<SwapChain> swapchain);

        private:
            friend class SwapChainNodesState;

            std::vector<Ref<IBinarySemaphore>>                         m_ImagePresentSemaphores;
            std::vector<std::pair<Ref<IFence>, Ref<IBinarySemaphore>>> m_FenceToPresentSemaphore;
            Ref<SwapChain>                                             m_SwapChain;
            bool                                                       m_OutOfDate = false;
        };

        SwapChainNodesState(Ref<Surface> surface);

        Ref<Iteration> GetCurrentIteration();
        Ref<IFence>    CreateFence();
        void           CreateNewIteration();

    private:
        Ref<Iteration> m_Iteration;
        Ref<Surface>   m_Surface;
        Ref<FencePool> m_FencePool;
    };

}  // namespace Engine::Vulkan::RenderGraph