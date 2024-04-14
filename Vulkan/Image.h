#pragma once

#include "Vulkan/Common.h"

namespace Vulkan {
    class Image {
    public:
        Image(Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Image();

        VkImage     Handle();
        Ref<Device> GetDevice();

    private:
        Ref<Device>    m_Device;
        uint32_t       m_Width;
        uint32_t       m_Height;
        VkImage        m_Image;
        VkDeviceMemory m_Memory;
    };
}  // namespace Vulkan