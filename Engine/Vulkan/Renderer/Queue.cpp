#include "Queue.h"

#include "Device.h"

namespace Engine::Vulkan {

    Queue::Queue(Device* device, VkQueue queue, uint32_t queue_family_index) : m_Device(device), m_Queue(queue), m_FamilyIndex(queue_family_index) {}
    Queue::~Queue() {
        WaitIdle();
    }

    void Queue::WaitIdle() const {
        PROFILER_SCOPE("Engine::Vulkan::Queue::WaitIdle");
        VK_CHECK(vkQueueWaitIdle(m_Queue));
    }

    const VkQueue& Queue::Handle() const {
        return m_Queue;
    }
    uint32_t Queue::FamilyIndex() const {
        return m_FamilyIndex;
    }
    Device* Queue::GetDevice() const {
        return m_Device;
    }
    Ref<Queue> Queue::CreateRef() {
        return Ref<Queue>(m_Device->shared_from_this(), this);
    }

}  // namespace Engine::Vulkan