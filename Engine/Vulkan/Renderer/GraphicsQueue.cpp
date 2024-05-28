#include "GraphicsQueue.h"

#include "Device.h"

#include "Engine/Vulkan/Interface/RawCommandBuffer.h"

namespace Engine::Vulkan {

    GraphicsQueue::GraphicsQueue(Device* device, VkQueue queue, uint32_t queue_family_index) : Queue(device, queue, queue_family_index) {}

    void GraphicsQueue::Submit(IRawCommandBuffer& cmdbuf, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore, VkFence signal_fence) {
        PROFILER_SCOPE("Engine::Vulkan::GraphicsQueue::Submit");
        VkCommandBufferSubmitInfo cmdbufInfo{};
        cmdbufInfo.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        cmdbufInfo.commandBuffer = cmdbuf.Handle();

        VkSemaphoreSubmitInfo waitInfo{};
        waitInfo.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        waitInfo.semaphore = wait_semaphore;
        waitInfo.stageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;

        VkSemaphoreSubmitInfo signalInfo{};
        signalInfo.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        signalInfo.semaphore = signal_semaphore;
        signalInfo.stageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;

        VkSubmitInfo2 info{};
        info.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        info.commandBufferInfoCount = 1;
        info.pCommandBufferInfos    = &cmdbufInfo;

        if (signal_semaphore != VK_NULL_HANDLE) {
            info.signalSemaphoreInfoCount = 1;
            info.pSignalSemaphoreInfos    = &signalInfo;
        }
        if (wait_semaphore != VK_NULL_HANDLE) {
            info.waitSemaphoreInfoCount = 1;
            info.pWaitSemaphoreInfos    = &waitInfo;
        }

        VK_CHECK(vkQueueSubmit2(Handle(), 1, &info, signal_fence));
    }
}  // namespace Engine::Vulkan