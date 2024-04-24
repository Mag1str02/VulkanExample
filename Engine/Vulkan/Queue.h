#pragma once

#include "Object.h"
#include "Task.h"

namespace Engine::Vulkan {
    class Queue : public Object {
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

    private:
        std::vector<Ref<Task>> m_RunningTasks;

        Device*  m_Device;
        VkQueue  m_Queue;
        uint32_t m_FamilyIndex;
    };
}  // namespace Engine::Vulkan