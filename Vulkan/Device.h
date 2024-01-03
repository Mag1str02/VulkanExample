#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"

class Window;

namespace Vulkan {
    class Instance;

    class Device {
    public:
        ~Device();

        VkDevice         GetLogicDevice();
        VkPhysicalDevice GetPhysicalDevice();

    private:
        friend class Instance;

        Device(VkPhysicalDevice device, Ref<Instance> instance);

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         m_LogicDevice    = VK_NULL_HANDLE;
        Ref<Instance>    m_Instance       = nullptr;
    };

};  // namespace Vulkan