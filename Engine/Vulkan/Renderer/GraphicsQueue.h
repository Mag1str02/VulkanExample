#pragma once

#include "Engine/Vulkan/Renderer/Queue.h"

namespace Engine::Vulkan {

    class GraphicsQueue : public Queue {
    public:
        class SubmitInfo {
        public:
            SubmitInfo() = default;

            void AddCommandBuffer(const IRawCommandBuffer& command_buffer);
            void AddWaitSemaphore(const IBinarySemaphore& semaphore);
            void AddSignalSemaphore(const IBinarySemaphore& semaphore);
            void SetSignalFence(const IFence& fence);

        private:
            friend class GraphicsQueue;

            std::vector<VkCommandBufferSubmitInfo> m_BufferSubmitInfos;
            std::vector<VkSemaphoreSubmitInfo>     m_WaitInfos;
            std::vector<VkSemaphoreSubmitInfo>     m_SignalInfos;
            VkFence                                m_SignalFence = VK_NULL_HANDLE;

            VkSubmitInfo2 m_SubmitInfo = {};
        };

        GraphicsQueue(Device* device, VkQueue queue, uint32_t queue_family_index);
        ~GraphicsQueue() = default;

        void Submit(const SubmitInfo& submit_info);
    };
}  // namespace Engine::Vulkan