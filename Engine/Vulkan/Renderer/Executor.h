#pragma once

#include "Device.h"
#include "Task.h"

namespace Engine::Vulkan {
    class Executor final : NonCopyMoveable {
    public:
        Executor(Ref<Device> device);
        ~Executor();

        void Submit(Ref<Task> task);

    private:
        Ref<Device> m_Device;
    };
}  // namespace Engine::Vulkan