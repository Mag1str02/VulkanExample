#include "Tracker.h"

#include "ImageTracker.h"

#include "Engine/Vulkan/Helpers.h"
#include "Engine/Vulkan/Interface/Image.h"
#include "Engine/Vulkan/Renderer/Executor.h"

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

    std::vector<VkImageMemoryBarrier2> Tracker::SynchronizeImages(Executor* executor) const {
        std::vector<VkImageMemoryBarrier2> result;
        for (const auto& [image, tracker] : m_ImageStates) {
            auto barrier = executor->UpdateImageState(*image, tracker);
            if (barrier.has_value()) {
                result.push_back(*barrier);
            }
        }
        return result;
    }

}  // namespace Engine::Vulkan::Synchronization