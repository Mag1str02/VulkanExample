#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Synchronization/ImageTracker.h"

namespace Engine::Vulkan::Interface {

    class Image {
    public:
        using SyncState = Synchronization::ImageTracker::State;

        Image()          = default;
        virtual ~Image() = default;

        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetWidth() const  = 0;

        virtual VkFormat          GetFormat() const     = 0;
        virtual VkImageUsageFlags GetUsageFlags() const = 0;
        virtual VkExtent2D        GetExtent() const     = 0;

        virtual VkImage     Handle() const    = 0;
        virtual Ref<Device> GetDevice() const = 0;

        virtual SyncState& GetSyncState()            = 0;
        virtual bool       SemaphoreRequired() const = 0;
    };
};  // namespace Engine::Vulkan::Interface