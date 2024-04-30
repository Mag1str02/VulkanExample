#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Synchronization {

    class Tracker {
    public:
        std::optional<VkImageMemoryBarrier2> RequestAccess(Ref<IImage> image, AccessScope scope, VkImageLayout layout);

        void Reset();

        bool HasImage(Ref<IImage> image) const;

    private:
        std::unordered_map<Ref<IImage>, ImageState> m_ImageStates;
    };

}  // namespace Engine::Vulkan::Synchronization