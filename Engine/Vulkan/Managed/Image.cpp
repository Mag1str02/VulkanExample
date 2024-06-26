#include "Image.h"

#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan::Managed {

    Image::Image(VkImage image, VkFormat format, VkImageUsageFlags usage_flags, VkExtent2D extent, Device* device) {
        Init(image, format, usage_flags, extent, device);
    }
    void Image::Init(VkImage image, VkFormat format, VkImageUsageFlags usage_flags, VkExtent2D extent, Device* device) {
        m_Image      = image;
        m_Format     = format;
        m_UsageFlags = usage_flags;
        m_Extent     = extent;
        m_Device     = device;
    }

    const VkImage& Image::Handle() const {
        return m_Image;
    }

    VkExtent2D Image::GetExtent() const {
        return m_Extent;
    }
    uint32_t Image::GetHeight() const {
        return m_Extent.height;
    }
    uint32_t Image::GetWidth() const {
        return m_Extent.width;
    }
    VkFormat Image::GetFormat() const {
        return m_Format;
    }
    Device* Image::GetDevice() const {
        return m_Device;
    }
    VkImageUsageFlags Image::GetUsageFlags() const {
        return m_UsageFlags;
    }
    Image::SyncState& Image::GetSyncState() {
        return m_SyncState;
    }

};  // namespace Engine::Vulkan::Managed