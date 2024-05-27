#include "SwapChainPresentPassNode.h"

#include "Engine/Vulkan/Interface/BinarySemaphore.h"
#include "Engine/Vulkan/Renderer/Device.h"
#include "Engine/Vulkan/Renderer/Executor.h"

namespace Engine::Vulkan::RenderGraph {

    bool SwapChainPresentPassNode::Cluster::IsCompleted() const {
        return true;
    }
    bool SwapChainPresentPassNode::Cluster::AddPass(IPass* pass) {
        if (!pass->Is<SwapChainPresentPassNode::Pass>()) {
            return false;
        }
        if (m_PresentPass == nullptr) {
            m_PresentPass = pass->As<SwapChainPresentPassNode::Pass>();
            return true;
        }
        return false;
    }
    void SwapChainPresentPassNode::Cluster::Submit(Executor* executor) {
        auto res = executor->GetDevice()->GetPresentationQueue()->Present(m_PresentPass->GetPresentImage(), m_PresentPass->GetWaitSemaphore());
    }

    void SwapChainPresentPassNode::Cluster::AddWaitSemaphore(Ref<IBinarySemaphore> semaphore) {
        DE_ASSERT(m_PresentPass != nullptr, "No present pass");
        m_PresentPass->SetWaitSemaphore(std::move(semaphore));
    }
    void SwapChainPresentPassNode::Cluster::AddSignalSemaphore(Ref<IBinarySemaphore> semaphore) {
        DE_ASSERT_FAIL("Cannot add singla semaphore to swapchain present pass cluster");
    }

    void SwapChainPresentPassNode::Pass::Prepare() {
        m_Iteration = m_State->GetCurrentIteration();
        m_Iteration->SetCurrentPresentSemaphore(m_WaitSemaphore);
        m_State.reset();
    }

    void SwapChainPresentPassNode::Pass::SetWaitSemaphore(Ref<IBinarySemaphore> semaphore) {
        DE_ASSERT(m_WaitSemaphore == nullptr, "Cannot set multiple wait semaphores for swapchain present pass");
        m_WaitSemaphore = std::move(semaphore);
    }

    VkSwapchainKHR SwapChainPresentPassNode::Pass::GetSwapChainHandle() {
        return m_Iteration->GetSwapChainHandle();
    }

    Ref<SwapChain::Image> SwapChainPresentPassNode::Pass::GetPresentImage() {
        return m_Iteration->GetCurrentImage();
    }
    VkSemaphore SwapChainPresentPassNode::Pass::GetWaitSemaphore() {
        return m_WaitSemaphore->Handle();
    }

    SwapChainPresentPassNode::Pass::Pass(Ref<SwapChainNodesState> state) : m_State(state) {}

    SwapChainPresentPassNode::SwapChainPresentPassNode(Ref<SwapChainNodesState> state) : m_State(std::move(state)) {}

    std::optional<std::string> SwapChainPresentPassNode::Validate() const {
        if (GetExternalResource(kPresentImageName, DependencyType::ReadOnlyInput) == nullptr) {
            return std::format("SwapChainAquirePassNode requires input node with name {}", kPresentImageName);
        }
        return std::nullopt;
    }

    Scope<IPass> SwapChainPresentPassNode::CreatePass() {
        return Scope<Pass>(new Pass(m_State));
    }

    Scope<IPassCluster> SwapChainPresentPassNode::CreatePassCluster() {
        return CreateScope<Cluster>();
    }
}  // namespace Engine::Vulkan::RenderGraph