#include "Tracker.h"

#include "State.h"

#include "Engine/Vulkan/Interface/Image.h"

namespace Engine::Vulkan::Synchronization {

    std::vector<VkImageMemoryBarrier2> Tracker::RequestAccess(Ref<IImage> image, AccessScope scope, VkImageLayout layout) {
        auto barriers = m_ImageStates[image].AccessRequest(scope, layout);
        for (auto& barrier : barriers) {
            barrier.image = image->Handle();
        }
        return barriers;
    }

}  // namespace Engine::Vulkan::Synchronization