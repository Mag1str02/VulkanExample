#pragma once

#include "Engine/Vulkan/Renderer/Queue.h"

namespace Engine::Vulkan {
    class GraphicsQueue : public Queue {
    public:
        GraphicsQueue(Device* device, VkQueue queue, uint32_t queue_family_index);
        ~GraphicsQueue() = default;

        void Submit(IRawCommandBuffer& cmdbuf,
                    VkSemaphore        wait_semaphore   = VK_NULL_HANDLE,
                    VkSemaphore        signal_semaphore = VK_NULL_HANDLE,
                    VkFence            signal_fence     = VK_NULL_HANDLE);
    };
}  // namespace Engine::Vulkan