#pragma once

#include "Task.h"

#include "Engine/Vulkan/Concrete/CommandBuffer.h"

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
        VkSemaphore      CreateSemaphore();
        void             DestroySemaphore(VkSemaphore semaphore);
        void             RemoveCompleted();
        Ref<CommandPool> GetPool();

        struct TaskContext {
            Ref<Task>                    task;
            Ref<Concrete::CommandBuffer> syncronization_buffer;
            VkSemaphore                  wait_semaphore;
            Ref<Concrete::Fence>         secondary_fence;
        };

    private:
        VkSemaphore              latest_semaphore = VK_NULL_HANDLE;
        std::vector<TaskContext> m_RunningTasks;
        Ref<CommandPool>         m_Pool;

        Device*  m_Device;
        VkQueue  m_Queue;
        uint32_t m_FamilyIndex;
    };
}  // namespace Engine::Vulkan