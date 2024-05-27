#pragma once

#include "Engine/Vulkan/Interface/Image.h"

namespace Engine::Vulkan::Managed {
    class Image : public IImage {
    public:
        Image(VkImage image, VkFormat format, VkImageUsageFlags usage_flags, VkExtent2D extent, Device* device);
        Image()          = default;
        virtual ~Image() = default;

        virtual VkFormat          GetFormat() const override;
        virtual VkImageUsageFlags GetUsageFlags() const override;
        virtual VkExtent2D        GetExtent() const override;
        virtual uint32_t          GetHeight() const override;
        virtual uint32_t          GetWidth() const override;

        virtual VkImage     Handle() const override;
        virtual Ref<Device> GetDevice() const override;

    protected:
        void Init(VkImage image, VkFormat format, VkImageUsageFlags usage_flags, VkExtent2D extent, Device* device);

    protected:
        VkImage           m_Image      = VK_NULL_HANDLE;
        VkFormat          m_Format     = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags m_UsageFlags = 0;
        VkExtent2D        m_Extent{};

        Device* m_Device;

    private:
        virtual SyncState& GetSyncState() override;

        SyncState m_SyncState;
    };

}  // namespace Engine::Vulkan::Managed