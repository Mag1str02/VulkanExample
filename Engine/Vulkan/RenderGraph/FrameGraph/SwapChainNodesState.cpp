#include "SwapChainNodesState.h"

#include "Engine/Vulkan/Renderer/Device.h"
#include "Engine/Vulkan/Window/Surface.h"
#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan::RenderGraph {

    SwapChainNodesState::Iteration::~Iteration() {
        // m_SwapChain->GetSurface()->GetDevice()->GetPresentationQueue()->WaitIdle();
    }

    bool SwapChainNodesState::Iteration::AquireNextImage(VkSemaphore signal_semaphore, VkFence fence) {
        auto res    = m_SwapChain->AcquireImage(signal_semaphore, fence);
        m_OutOfDate = res == VK_SUBOPTIMAL_KHR;
        return res != VK_ERROR_OUT_OF_DATE_KHR;
    }

    void SwapChainNodesState::Iteration::SetOutOfDate() {
        m_OutOfDate = true;
    }
    bool SwapChainNodesState::Iteration::IsOutOfDate() const {
        return m_OutOfDate;
    }

    Ref<SwapChain::Image> SwapChainNodesState::Iteration::GetCurrentImage() const {
        return m_SwapChain->GetCurrentImage();
    }
    VkSwapchainKHR SwapChainNodesState::Iteration::GetSwapChainHandle() const {
        return m_SwapChain->Handle();
    }

    SwapChainNodesState::Iteration::Iteration(Ref<SwapChain> swapchain) : m_SwapChain(swapchain) {}

    SwapChainNodesState::SwapChainNodesState(Ref<Surface> surface) : m_Surface(surface) {
        m_Iteration = Ref<Iteration>(new Iteration(SwapChain::Create(m_Surface, nullptr)));
    }

    Ref<SwapChainNodesState::Iteration> SwapChainNodesState::GetCurrentIteration() {
        return m_Iteration;
    }

    void SwapChainNodesState::CreateNewIteration() {
        m_Iteration = Ref<Iteration>(new Iteration(SwapChain::Create(m_Surface, m_Iteration->m_SwapChain)));
    }

}  // namespace Engine::Vulkan::RenderGraph