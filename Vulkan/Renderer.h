#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"
#include "Queue.h"

class Window;

namespace Vulkan {

    class Debugger;
    class Instance;
    class Device;
    class Queue;

    class Renderer {
    public:
        Renderer();
        ~Renderer() = default;

        Ref<Instance> GetInstance();
        Ref<Device>   GetDevice();
        Ref<Queue>    GetGraphicsQueue();
        Ref<Queue>    GetPresentationQueue();

        VkInstance       GetInstanceHandle();
        VkDevice         GetLogicDevice();
        VkPhysicalDevice GetPhysicalDevice();

        NO_COPY_CONSTRUCTORS(Renderer);
        NO_MOVE_CONSTRUCTORS(Renderer);

    private:
        Ref<Instance> m_Instance;
        Ref<Debugger> m_Debugger;
        Ref<Device>   m_Device;
        Ref<Queue>    m_GraphicsQueue;
        Ref<Queue>    m_PresentationQueue;
    };

}  // namespace Vulkan