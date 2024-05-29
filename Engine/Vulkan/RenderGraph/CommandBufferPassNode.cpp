#include "CommandBufferPassNode.h"

#include "Engine/Vulkan/CommandBuffer.h"
#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Fence.h"
#include "Engine/Vulkan/Interface/Fence.h"
#include "Engine/Vulkan/Renderer/Device.h"
#include "Engine/Vulkan/Renderer/Executor.h"

namespace Engine::Vulkan::RenderGraph {

    CommandBufferPassNode::CommandBufferPassNode(Ref<Device> device) : m_CommandPool(CommandPool::Create(device->GetGraphicsQueue()->CreateRef())) {}

    std::optional<std::string> CommandBufferPassNode::Validate() const {
        return std::nullopt;
    }

    Scope<IPass> CommandBufferPassNode::CreatePass() {
        return CreateScope<Pass>(this);
    };
    Scope<IPassCluster> CommandBufferPassNode::CreatePassCluster() {
        return CreateScope<Cluster>();
    }

    CommandBufferPassNode::Pass::Pass(CommandBufferPassNode* node) : m_CommandBuffer(CommandBuffer::Create(node->m_CommandPool)) {}
    void CommandBufferPassNode::Pass::Record(CommandBuffer& command_buffer) {}
    void CommandBufferPassNode::Pass::Prepare() {
        PROFILER_SCOPE("Engine::Vulkan::RenderGraph::CommandBufferPassNode::Pass::Prepare");

        m_CommandBuffer->Begin();
        Record(*m_CommandBuffer);
    }

    void CommandBufferPassNode::Cluster::Submit(Executor* executor) {
        PROFILER_SCOPE("Engine::Vulkan::RenderGraph::CommandBufferPassNode::Cluster::Submit");

        DE_CHECK(!m_Passes.empty());

        m_Fence              = CreateRef<Fence>(executor->GetDevice()->shared_from_this());
        m_PatchCommandBuffer = executor->GeneratePatchCommandBuffer(m_Passes.front()->m_CommandBuffer->GetSynchronizationTracker());
        for (size_t i = 0; i + 1 < m_Passes.size(); ++i) {
            executor->RecordSynchronizationBarriers(*m_Passes[i]->m_CommandBuffer, m_Passes[i + 1]->m_CommandBuffer->GetSynchronizationTracker());
        }
        for (size_t i = 0; i < m_Passes.size(); ++i) {
            m_Passes[i]->m_CommandBuffer->End();
        }

        GraphicsQueue::SubmitInfo submit_info;
        for (const auto& semaphore : m_WaitSemaphores) {
            submit_info.AddWaitSemaphore(*semaphore);
        }
        for (const auto& semaphore : m_SignalSemaphores) {
            submit_info.AddSignalSemaphore(*semaphore);
        }
        submit_info.SetSignalFence(*m_Fence);

        if (m_PatchCommandBuffer != nullptr) {
            submit_info.AddCommandBuffer(*m_PatchCommandBuffer);
        }
        for (const auto& pass : m_Passes) {
            submit_info.AddCommandBuffer(pass->m_CommandBuffer->GetRawCommandBuffer());
        }

        executor->GetDevice()->GetGraphicsQueue()->Submit(submit_info);
    }

    bool CommandBufferPassNode::Cluster::AddPass(IPass* pass) {
        if (!pass->Is<Pass>()) {
            return false;
        }
        m_Passes.push_back(pass->As<Pass>());
        return true;
    }

    void CommandBufferPassNode::Cluster::AddWaitSemaphore(Ref<IBinarySemaphore> semaphore) {
        m_WaitSemaphores.emplace_back(std::move(semaphore));
    }
    void CommandBufferPassNode::Cluster::AddSignalSemaphore(Ref<IBinarySemaphore> semaphore) {
        m_SignalSemaphores.emplace_back(std::move(semaphore));
    }
    bool CommandBufferPassNode::Cluster::IsCompleted() const {
        return m_Fence->IsSignaled();
    }

}  // namespace Engine::Vulkan::RenderGraph