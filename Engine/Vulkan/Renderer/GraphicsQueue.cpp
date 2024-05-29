#include "GraphicsQueue.h"

#include "Device.h"

#include "Engine/Vulkan/Interface/BinarySemaphore.h"
#include "Engine/Vulkan/Interface/Fence.h"
#include "Engine/Vulkan/Interface/RawCommandBuffer.h"

namespace Engine::Vulkan {

    GraphicsQueue::GraphicsQueue(Device* device, VkQueue queue, uint32_t queue_family_index) : Queue(device, queue, queue_family_index) {}

    void GraphicsQueue::SubmitInfo::AddCommandBuffer(const IRawCommandBuffer& command_buffer) {
        VkCommandBufferSubmitInfo info{};
        info.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        info.commandBuffer = command_buffer.Handle();

        m_BufferSubmitInfos.push_back(info);
    }
    void GraphicsQueue::SubmitInfo::AddWaitSemaphore(const IBinarySemaphore& semaphore) {
        VkSemaphoreSubmitInfo info{};
        info.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        info.semaphore = semaphore.Handle();
        info.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;

        m_WaitInfos.push_back(info);
    }
    void GraphicsQueue::SubmitInfo::AddSignalSemaphore(const IBinarySemaphore& semaphore) {
        VkSemaphoreSubmitInfo info{};
        info.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        info.semaphore = semaphore.Handle();
        info.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;

        m_SignalInfos.push_back(info);
    }
    void GraphicsQueue::SubmitInfo::SetSignalFence(const IFence& fence) {
        m_SignalFence = fence.Handle();
    }

    void GraphicsQueue::Submit(const SubmitInfo& submit_info) {
        PROFILER_SCOPE("Engine::Vulkan::GraphicsQueue::Submit");

        VkSubmitInfo2 info{};
        info.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        info.commandBufferInfoCount   = submit_info.m_BufferSubmitInfos.size();
        info.pCommandBufferInfos      = submit_info.m_BufferSubmitInfos.data();
        info.signalSemaphoreInfoCount = submit_info.m_SignalInfos.size();
        info.pSignalSemaphoreInfos    = submit_info.m_SignalInfos.data();
        info.waitSemaphoreInfoCount   = submit_info.m_WaitInfos.size();
        info.pWaitSemaphoreInfos      = submit_info.m_WaitInfos.data();

        VK_CHECK(vkQueueSubmit2(Handle(), 1, &info, submit_info.m_SignalFence));
    }
}  // namespace Engine::Vulkan