#include "SwapChainAquirePassNode.h"

#include "Engine/Vulkan/Interface/BinarySemaphore.h"

namespace Engine::Vulkan::RenderGraph {

    void SwapChainAquirePassNode::Cluster::Submit(Executor* executor) {}
    bool SwapChainAquirePassNode::Cluster::AddPass(IPass* pass) {
        if (!pass->Is<SwapChainAquirePassNode::Pass>()) {
            return false;
        }
        if (m_AquirePass == nullptr) {
            m_AquirePass = pass->As<SwapChainAquirePassNode::Pass>();
            return true;
        }
        return false;
    }
    void SwapChainAquirePassNode::Cluster::AddWaitSemaphore(Ref<IBinarySemaphore> semaphore) {
        DE_ASSERT_FAIL("Cannot add wait semaphore for swapchain aquire cluster");
    }
    void SwapChainAquirePassNode::Cluster::AddSignalSemaphore(Ref<IBinarySemaphore> semaphore) {
        m_AquirePass->SetSignalSemaphore(semaphore);
    }

    bool SwapChainAquirePassNode::Cluster::IsCompleted() const {
        return m_AquirePass->IsCompleted();
    }

    SwapChainAquirePassNode::Pass::Pass(Ref<SwapChainNodesState> state) : m_State(state) {}

    void SwapChainAquirePassNode::Pass::SetSignalSemaphore(Ref<IBinarySemaphore> semaphore) {
        DE_ASSERT(m_SignalSemaphore == nullptr, "Cannot sey multiple signal semaphores for swapchain aquire pass");
        m_SignalSemaphore = std::move(semaphore);
    }
    bool SwapChainAquirePassNode::Pass::IsCompleted() const {
        return m_SignalFence->IsSignaled();
    }
    void SwapChainAquirePassNode::Pass::Prepare() {
        DE_ASSERT(m_SignalSemaphore != nullptr, "No semaphore");
        m_SignalFence = m_State->CreateFence();
        if (m_State->GetCurrentIteration()->IsOutOfDate()) {
            m_State->CreateNewIteration();
        }
        while (!m_State->GetCurrentIteration()->AquireNextImage(m_SignalSemaphore->Handle(), m_SignalFence->Handle())) {
            m_State->CreateNewIteration();
        }
        m_Iteration = m_State->GetCurrentIteration();
        m_State.reset();
    }

    SwapChainAquirePassNode::SwapChainAquirePassNode(Ref<SwapChainNodesState> state) : m_State(std::move(state)) {}

    std::optional<std::string> SwapChainAquirePassNode::Validate() const {
        if (GetExternalResource(kAquireImageName, DependencyType::Output) == nullptr) {
            return std::format("SwapChainAquirePassNode requires output node with name {}", kAquireImageName);
        }
        return std::nullopt;
    }

    Scope<IPass> SwapChainAquirePassNode::CreatePass() {
        return Scope<Pass>(new Pass(m_State));
    }
    Scope<IPassCluster> SwapChainAquirePassNode::CreatePassCluster() {
        return Scope<Cluster>(new Cluster());
    }
}  // namespace Engine::Vulkan::RenderGraph