#pragma once

#include "Engine/Vulkan/Managed/Image.h"

namespace Engine::Vulkan {
    class Image : public Managed::Image {
    public:
        static Ref<Image> Create(
            Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Image();

    private:
        Image(Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

    private:
        Ref<Device>    m_Device;
        VkDeviceMemory m_Memory;
    };
};  // namespace Engine::Vulkan