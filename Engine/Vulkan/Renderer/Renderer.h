#pragma once

#include "Config.h"

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        Ref<Instance> GetInstance();
        Ref<Device>   GetDevice();
        Ref<Queue>    GetQueue();

        NO_COPY_CONSTRUCTORS(Renderer);
        NO_MOVE_CONSTRUCTORS(Renderer);

    private:
        Config        m_Config;
        Ref<Instance> m_Instance;
        Ref<Debugger> m_Debugger;
        Ref<Device>   m_Device;
    };

}  // namespace Engine::Vulkan