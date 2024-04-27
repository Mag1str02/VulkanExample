#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Synchronization {

    class Tracker {
    public:
        std::vector<VkImageMemoryBarrier2> RequestAccess(Ref<IImage> image, AccessScope scope, VkImageLayout layout);

    private:
        std::unordered_map<Ref<IImage>, ImageState> m_ImageStates;
    };

}  // namespace Engine::Vulkan::Synchronization