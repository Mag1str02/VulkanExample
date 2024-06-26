#pragma once

#include "ImageTracker.h"

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Synchronization {

    class Tracker {
    public:
        std::optional<VkImageMemoryBarrier2> RequestAccess(Ref<IImage> image, AccessScope scope, VkImageLayout layout);

        void Reset();

        std::vector<VkImageMemoryBarrier2> SynchronizeImages(Executor* executor) const;

    private:
        std::unordered_map<Ref<IImage>, ImageTracker> m_ImageStates;
    };

}  // namespace Engine::Vulkan::Synchronization