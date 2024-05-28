#include "CommandPool.h"

#include "Engine/Vulkan/Renderer/Queue.h"

namespace Engine::Vulkan::Managed {

    CommandPool::CommandPool(Queue* queue, VkCommandPool handle) {
        Init(queue, handle);
    }

    const VkCommandPool& CommandPool::Handle() const {
        return m_Handle;
    }
    Queue* CommandPool::GetQueue() const {
        return m_Queue;
    }

    void CommandPool::Init(Queue* queue, VkCommandPool handle) {
        m_Queue  = queue;
        m_Handle = handle;
    }

}  // namespace Engine::Vulkan::Managed