#pragma once

#include "Device.h"

namespace Engine::Vulkan {
    class Executor final : NonCopyMoveable {
    public:
        Executor(Ref<Device> device);
        ~Executor();

        void        Submit(Ref<ITask> task);
        Ref<Device> GetDevice();

    private:
        void RemoveCompleted();

    private:
        Ref<Device>             m_Device;
        std::vector<Ref<ITask>> m_RunningTasks;
    };
}  // namespace Engine::Vulkan