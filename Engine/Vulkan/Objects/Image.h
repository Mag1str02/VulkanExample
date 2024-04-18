#pragma once

#include "Object.h"

namespace Engine::Vulkan {
    class IImage {
    public:
        IImage()          = default;
        virtual ~IImage() = default;

        VkImage           Handle();
        VkFormat          GetFormat() const;
        VkImageUsageFlags GetUsageFlags() const;
        VkExtent2D        GetExtent() const;

        uint32_t    GetHeight() const;
        uint32_t    GetWidth() const;
        Ref<Device> GetDevice() const;

    protected:
        VkImage           m_Image      = VK_NULL_HANDLE;
        VkFormat          m_Format     = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags m_UsageFlags = 0;
        VkExtent2D        m_Extent;

        Ref<Device> m_Device;
    };

    class Image : public IImage, public Object {
    public:
        Image(Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Image();

    private:
        VkDeviceMemory m_Memory;
    };
}  // namespace Engine::Vulkan