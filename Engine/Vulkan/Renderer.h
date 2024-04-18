#pragma once

#include "Common.h"
#include "Debugger.h"
#include "Device.h"
#include "Instance.h"

namespace Engine::Vulkan {

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        VkInstance       GetInstanceHandle();
        VkDevice         GetLogicDevice();
        VkPhysicalDevice GetPhysicalDevice();
        VkQueue          GetQueue();
        uint32_t         GetQueueFamilyIndex();

        NO_COPY_CONSTRUCTORS(Renderer);
        NO_MOVE_CONSTRUCTORS(Renderer);

    private:
        Config           m_Config;
        Handle<Instance> m_Instance;
        Handle<Debugger> m_Debugger;
        Handle<Device>   m_Device;
    };

}  // namespace Engine::Vulkan