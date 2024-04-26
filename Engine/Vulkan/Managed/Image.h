#pragma once

#include "Engine/Vulkan/Interface/Image.h"
#include "Engine/Vulkan/Sync/State.h"

namespace Engine::Vulkan::Managed {
    class Image : public Interface::Image {
    public:
        Image()          = default;
        virtual ~Image() = default;

        void Init(VkImage image, VkFormat format, VkImageUsageFlags usage_flags, VkExtent2D extent, Ref<Device> device);

        virtual VkFormat          GetFormat() const override;
        virtual VkImageUsageFlags GetUsageFlags() const override;
        virtual VkExtent2D        GetExtent() const override;
        virtual uint32_t          GetHeight() const override;
        virtual uint32_t          GetWidth() const override;

        virtual VkImage           Handle() const override;
        virtual Ref<Device>       GetDevice() const override;
        virtual Sync::ImageState& GetSyncState() override;

    protected:
        VkImage           m_Image      = VK_NULL_HANDLE;
        VkFormat          m_Format     = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags m_UsageFlags = 0;
        VkExtent2D        m_Extent{};

        Sync::ImageState m_SyncState;

        Ref<Device> m_Device;
    };

}  // namespace Engine::Vulkan::Managed