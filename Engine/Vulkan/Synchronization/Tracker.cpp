#include "Tracker.h"

#include "State.h"

#include "Engine/Vulkan/Helpers.h"
#include "Engine/Vulkan/Interface/Image.h"

namespace Engine::Vulkan::Synchronization {

    std::vector<VkImageMemoryBarrier2> Tracker::RequestAccess(Ref<IImage> image, AccessScope scope, VkImageLayout layout) {
        auto barriers = m_ImageStates[image].AccessRequest(scope, layout);
        for (auto& barrier : barriers) {
            barrier.image                       = image->Handle();
            barrier.subresourceRange.aspectMask = Helpers::ImageAspectFlagsFromFormat(image->GetFormat());
        }
        return barriers;
    }

    void Tracker::Reset() {
        m_ImageStates.clear();
    }

    bool Tracker::HasImage(Ref<IImage> image) const {
        return m_ImageStates.contains(std::move(image));
    }

}  // namespace Engine::Vulkan::Synchronization