#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"

namespace Vulkan {
    class Device;

    class Queue {
    public:
        enum class Family {
            Unknown = 0,
            Graphics,
            Presentation,
        };

        VkQueue Handle();

    private:
        friend class Device;

        Queue(Ref<Device> device, VkQueue handle);

        VkQueue     m_Handle = VK_NULL_HANDLE;
        Ref<Device> m_Device = nullptr;
    };
}  // namespace Vulkan