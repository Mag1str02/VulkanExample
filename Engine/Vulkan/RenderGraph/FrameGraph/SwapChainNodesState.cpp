#include "SwapChainNodesState.h"

#include "Engine/Vulkan/Renderer/Device.h"
#include "Engine/Vulkan/Window/Surface.h"
#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan::RenderGraph {

    SwapChainNodesState::Iteration::~Iteration() {
        m_SwapChain->GetSurface()->GetDevice()->GetPresentationQueue()->WaitIdle();
    }

    bool SwapChainNodesState::Iteration::AquireNextImage(VkSemaphore signal_semaphore, VkFence fence) {
        auto res    = m_SwapChain->AcquireImage(signal_semaphore, fence);
        m_OutOfDate = res == VK_SUBOPTIMAL_KHR;
        return res != VK_ERROR_OUT_OF_DATE_KHR;
    }

    void SwapChainNodesState::Iteration::SetCurrentPresentSemaphore(Ref<IBinarySemaphore> semaphore) {
        m_ImagePresentSemaphores[m_SwapChain->GetCurrentImage()->GetIndex()] = std::move(semaphore);
    }
    void SwapChainNodesState::Iteration::SetCurrentAquireFence(Ref<IFence> fence) {
        auto& sema = m_ImagePresentSemaphores[GetCurrentImage()->GetIndex()];
        if (sema != nullptr) {
            m_FenceToPresentSemaphore.emplace_back(std::move(fence), std::move(sema));
        }
        for (int64_t i = 0; i < m_FenceToPresentSemaphore.size(); ++i) {
            if (m_FenceToPresentSemaphore[i].first->IsSignaled()) {
                m_FenceToPresentSemaphore.erase(m_FenceToPresentSemaphore.begin() + i);
                --i;
            }
        }
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

    SwapChainNodesState::Iteration::Iteration(Ref<SwapChain> swapchain) : m_SwapChain(swapchain) {
        m_ImagePresentSemaphores.resize(m_SwapChain->GetImageCount());
    }

    SwapChainNodesState::SwapChainNodesState(Ref<Surface> surface) : m_Surface(surface) {
        m_Iteration = Ref<Iteration>(new Iteration(SwapChain::Create(m_Surface, nullptr)));
        m_FencePool = FencePool::Create(surface->GetDevice());
    }

    Ref<SwapChainNodesState::Iteration> SwapChainNodesState::GetCurrentIteration() {
        return m_Iteration;
    }

    Ref<IFence> SwapChainNodesState::CreateFence() {
        return m_FencePool->CreateFence();
    }
    void SwapChainNodesState::CreateNewIteration() {
        m_Iteration = Ref<Iteration>(new Iteration(SwapChain::Create(m_Surface, m_Iteration->m_SwapChain)));
    }

}  // namespace Engine::Vulkan::RenderGraph