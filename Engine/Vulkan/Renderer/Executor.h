#pragma once

#include "Device.h"
#include "Task.h"

namespace Engine::Vulkan {
    class Executor final : NonCopyMoveable {
    public:
        Executor(Ref<Device> device);
        ~Executor();

        void        Submit(Ref<Task> task);
        Ref<Device> GetDevice();

    private:
        void RemoveCompleted();

    private:
        Ref<Device>            m_Device;
        std::vector<Ref<Task>> m_RunningTasks;
    };
}  // namespace Engine::Vulkan