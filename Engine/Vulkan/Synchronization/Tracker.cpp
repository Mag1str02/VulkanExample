#include "Tracker.h"

#include "ImageTracker.h"

#include "Engine/Vulkan/Helpers.h"
#include "Engine/Vulkan/Interface/Image.h"

namespace Engine::Vulkan::Synchronization {

    std::optional<VkImageMemoryBarrier2> Tracker::RequestAccess(Ref<IImage> image, AccessScope scope, VkImageLayout layout) {
        PROFILER_SCOPE("Engine::Vulkan::Synchronization::Tracker::RequestAccess");
        auto barrier = m_ImageStates[image].AccessRequest(scope, layout);
        if (barrier) {
            barrier->image                       = image->Handle();
            barrier->subresourceRange.aspectMask = Helpers::ImageAspectFlagsFromFormat(image->GetFormat());
        };
        return barrier;
    }

    void Tracker::Reset() {
        m_ImageStates.clear();
    }

    bool Tracker::HasImage(Ref<IImage> image) const {
        return m_ImageStates.contains(std::move(image));
    }

}  // namespace Engine::Vulkan::Synchronization