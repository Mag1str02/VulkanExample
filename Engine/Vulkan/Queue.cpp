#include "Queue.h"

#include "Device.h"

namespace Engine::Vulkan {
    Queue::Queue(Device* device, VkQueue queue, uint32_t queue_family_index) : m_Device(device), m_Queue(queue), m_FamilyIndex(queue_family_index) {}
    Queue::~Queue() {
        WaitIdle();
    }

    void Queue::WaitIdle() {
        PROFILER_SCOPE("Engine::Vulkan::Queue::WaitIdle");
        vkQueueWaitIdle(m_Queue);
    }
    void Queue::Submit(Ref<Task> task) {
        PROFILER_SCOPE("Engine::Vulkan::Queue::Submit");
        task->Run(m_Queue);
        m_RunningTasks.push_back(task);
        RemoveCompleted();
    }
    void Queue::RemoveCompleted() {
        for (size_t i = 0; i < m_RunningTasks.size(); ++i) {
            if (m_RunningTasks[i]->IsCompleted()) {
                m_RunningTasks.erase(m_RunningTasks.begin() + i);
                --i;
            }
        }
    }

    VkQueue Queue::Handle() {
        return m_Queue;
    }
    uint32_t Queue::FamilyIndex() {
        return m_FamilyIndex;
    }
    Ref<Device> Queue::GetDevice() {
        return Ref<Device>(m_Device->shared_from_this(), m_Device);
    }

}  // namespace Engine::Vulkan