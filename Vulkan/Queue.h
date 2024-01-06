#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"

namespace Vulkan {

    class Queue {
    public:
        VkQueue Handle();

    private:
        friend class Device;

        Queue(Ref<Device> device, VkQueue handle);

        VkQueue     m_Handle = VK_NULL_HANDLE;
        Ref<Device> m_Device = nullptr;
    };

}  // namespace Vulkan