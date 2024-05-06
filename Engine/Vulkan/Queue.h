#pragma once

#include "Task.h"

namespace Engine::Vulkan {
    class Queue : NonCopyMoveable {
    public:
        Queue(Device* device, VkQueue queue, uint32_t queue_family_index);
        ~Queue();

        void Submit(Ref<Task> task);
        void WaitIdle();

        Ref<Device> GetDevice();
        uint32_t    FamilyIndex();
        VkQueue     Handle();

    private:
        void RemoveCompleted();

        struct TaskContext {
            Ref<Task>       task;
            VkCommandBuffer syncronization_buffer;
        };

    private:
        std::vector<TaskContext> m_RunningTasks;
        VkCommandPool            m_Pool;

        Device*  m_Device;
        VkQueue  m_Queue;
        uint32_t m_FamilyIndex;
    };
}  // namespace Engine::Vulkan