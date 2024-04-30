#include "Queue.h"

#include "Device.h"

#include "Engine/Vulkan/Interface/Fence.h"

namespace Engine::Vulkan {
    Queue::Queue(Device* device, VkQueue queue, uint32_t queue_family_index) : m_Device(device), m_Queue(queue), m_FamilyIndex(queue_family_index) {
        VkCommandPoolCreateInfo info{};
        info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.queueFamilyIndex = m_FamilyIndex;
        info.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        VK_CHECK(vkCreateCommandPool(m_Device->GetLogicDevice(), &info, nullptr, &m_Pool));
    }
    Queue::~Queue() {
        WaitIdle();
        RemoveCompleted();
        DE_ASSERT(m_RunningTasks.empty(), "Unfinished tasks");

        vkDestroyCommandPool(m_Device->GetLogicDevice(), m_Pool, nullptr);
    }

    void Queue::WaitIdle() {
        PROFILER_SCOPE("Engine::Vulkan::Queue::WaitIdle");
        vkQueueWaitIdle(m_Queue);
    }
    void Queue::Submit(Ref<Task> task) {
        PROFILER_SCOPE("Engine::Vulkan::Queue::Submit");

        TaskContext ctx;
        ctx.task = task;

        task->Run(m_Queue);
        m_RunningTasks.emplace_back(std::move(ctx));

        RemoveCompleted();
    }
    void Queue::RemoveCompleted() {
        for (size_t i = 0; i < m_RunningTasks.size(); ++i) {
            if (m_RunningTasks[i].task->GetFence()->IsSignaled()) {
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