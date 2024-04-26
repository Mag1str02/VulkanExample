#pragma once

#include "Common.h"
#include "Config.h"
#include "Device.h"
#include "Task.h"
#include "CommandPool.h"

#include "Engine/Vulkan/Interface/Image.h"

namespace Engine::Vulkan {

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        Ref<Instance> GetInstance();
        Ref<Device>   GetDevice();
        Ref<Queue>    GetQueue();

        void Submit(Ref<Task> task);

        NO_COPY_CONSTRUCTORS(Renderer);
        NO_MOVE_CONSTRUCTORS(Renderer);

    private:
        Config        m_Config;
        Ref<Instance> m_Instance;
        Ref<Debugger> m_Debugger;
        Ref<Device>   m_Device;
        Ref<Queue>    m_Queue;
    };

}  // namespace Engine::Vulkan