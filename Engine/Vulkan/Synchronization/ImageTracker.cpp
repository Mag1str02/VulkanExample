#include "ImageTracker.h"

#include "Engine/Vulkan/Helpers.h"
#include "Engine/Vulkan/Interface/Image.h"

namespace Engine::Vulkan::Synchronization {

    namespace {
        static constexpr VkImageSubresourceRange kDefaultImageResourceRange = {
            .baseMipLevel   = 0,
            .levelCount     = VK_REMAINING_MIP_LEVELS,
            .baseArrayLayer = 0,
            .layerCount     = VK_REMAINING_ARRAY_LAYERS,
        };

        VkImageMemoryBarrier2 CreateImageBarrier() {
            VkImageMemoryBarrier2 barrier{};
            barrier.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.subresourceRange = kDefaultImageResourceRange;
            return barrier;
        }

    }  // namespace

    VkPipelineStageFlags2 ImageTracker::ReadAccesses::GetStages() const {
        return m_Stages;
    }

    const ImageTracker::ReadAccesses::Readers& ImageTracker::ReadAccesses::GetReaders() const {
        return m_Readers;
    }

    std::pair<VkAccessFlags2, VkPipelineStageFlags2> ImageTracker::ReadAccesses::AddAccess(const AccessScope& scope) {
        VkAccessFlags2        access = VK_ACCESS_2_NONE;
        VkPipelineStageFlags2 stages = VK_PIPELINE_STAGE_2_NONE;

        if (scope.GetAccess() == VK_ACCESS_2_NONE) {
            return {access, stages};
        }

        auto it = m_Readers.find(scope.GetReadAccess());
        if (it == m_Readers.end()) {
            access = scope.GetReadAccess();
            stages = scope.GetStages();

            m_Stages |= stages;
            m_Readers[scope.GetReadAccess()] = scope.GetStages();
        } else if ((it->second & scope.GetStages()) != scope.GetStages()) {
            access = scope.GetReadAccess();
            stages = (it->second & scope.GetStages()) ^ scope.GetStages();

            m_Stages |= stages;
            it->second |= scope.GetStages();
        }
        return {access, stages};
    }

    std::optional<VkImageMemoryBarrier2> ImageTracker::AccessRequest(AccessScope scope, VkImageLayout layout) {
        PROFILER_SCOPE("Engine::Vulkan::Synchronization::ImageTracker::AccessRequest");

        DE_ASSERT(layout != VK_IMAGE_LAYOUT_UNDEFINED, "Access with image layout undefined is not supported");

        // Sets initial layout on first access request
        if (m_Prefix.layout == VK_IMAGE_LAYOUT_UNDEFINED) {
            m_Prefix.layout = layout;
        }

        bool is_write = scope.GetWriteAccess() != 0 || GetCurrentLayout() != layout;

        std::optional<VkImageMemoryBarrier2> result;
        VkImageMemoryBarrier2                barrier = CreateImageBarrier();
        barrier.oldLayout                            = GetCurrentLayout();
        barrier.newLayout                            = layout;
        if (m_Suffix.has_value()) {
            if (is_write) {
                barrier.srcAccessMask = m_Suffix->write.GetWriteAccess();  // TODO: Maybe access op should only be performed once
                barrier.srcStageMask  = m_Suffix->write.GetStages() | m_Suffix->read_accesses.GetStages();

                barrier.oldLayout = GetCurrentLayout();
                barrier.newLayout = layout;

                barrier.dstAccessMask = scope.GetAccess();
                barrier.dstStageMask  = scope.GetStages();

                result = barrier;

                m_Suffix         = State();
                m_Suffix->write  = scope;
                m_Suffix->layout = layout;
            } else {
                auto [access, stages] = m_Suffix->read_accesses.AddAccess(scope);

                if (stages != VK_PIPELINE_STAGE_2_NONE || access != VK_ACCESS_2_NONE) {
                    barrier.srcAccessMask = m_Suffix->write.GetWriteAccess();
                    barrier.srcStageMask  = m_Suffix->write.GetReadAccess();

                    barrier.dstAccessMask = access;
                    barrier.dstStageMask  = stages;

                    result = barrier;
                }
            }
        } else {
            if (is_write) {
                barrier.srcStageMask = m_Prefix.read_accesses.GetStages();
                barrier.dstStageMask = scope.GetStages();

                barrier.oldLayout = GetCurrentLayout();
                barrier.newLayout = layout;

                result = barrier;

                m_Prefix.write = scope;

                m_Suffix         = State();
                m_Suffix->write  = scope;
                m_Suffix->layout = layout;
            } else {
                m_Prefix.read_accesses.AddAccess(scope);
            }
        }

        return result;
    }

    const ImageTracker::State& ImageTracker::GetPrefix() const {
        return m_Prefix;
    }
    const std::optional<ImageTracker::State>& ImageTracker::GetSuffix() const {
        return m_Suffix;
    }

    VkImageLayout ImageTracker::GetCurrentLayout() const {
        if (m_Suffix.has_value()) {
            return m_Suffix->layout;
        }
        return m_Prefix.layout;
    }

    std::vector<VkImageMemoryBarrier2> ConnectSyncStates(const IImage& image, const ImageTracker::State& suffix, const ImageTracker::State& prefix) {
        std::vector<VkImageMemoryBarrier2> res;

        auto create_barrier = [&]() {
            VkImageMemoryBarrier2 barrier       = CreateImageBarrier();
            barrier.image                       = image.Handle();
            barrier.subresourceRange.aspectMask = Helpers::ImageAspectFlagsFromFormat(image.GetFormat());
            return barrier;
        };

        ImageTracker::State current_suffix = suffix;

        if (current_suffix.layout == prefix.layout) {
            for (const auto& [access, stages] : prefix.read_accesses.GetReaders()) {
                std::println("Reader access");
                auto [sync_access, sync_stages] = current_suffix.read_accesses.AddAccess({stages, access});
                if (sync_access != VK_ACCESS_2_NONE || sync_stages != VK_PIPELINE_STAGE_2_NONE) {
                    auto barrier = create_barrier();

                    barrier.srcAccessMask = current_suffix.write.GetWriteAccess();
                    barrier.srcStageMask  = current_suffix.write.GetStages();

                    barrier.oldLayout = current_suffix.layout;
                    barrier.newLayout = prefix.layout;

                    barrier.dstAccessMask = sync_access;
                    barrier.dstStageMask  = sync_stages;

                    res.push_back(barrier);
                }
            }
            {
                auto barrier = create_barrier();

                barrier.srcAccessMask = current_suffix.write.GetWriteAccess();
                barrier.srcStageMask  = current_suffix.write.GetStages();

                barrier.oldLayout = current_suffix.layout;
                barrier.newLayout = prefix.layout;

                barrier.dstAccessMask = prefix.write.GetAccess();
                barrier.dstStageMask  = prefix.write.GetStages();

                res.push_back(barrier);
            }
            {
                auto barrier = create_barrier();

                barrier.oldLayout = current_suffix.layout;
                barrier.newLayout = prefix.layout;

                barrier.srcStageMask = current_suffix.read_accesses.GetStages();
                barrier.dstStageMask = prefix.write.GetStages();

                res.push_back(barrier);
            }
        } else {
            auto barrier = create_barrier();

            barrier.srcAccessMask = current_suffix.write.GetWriteAccess();
            barrier.srcStageMask  = current_suffix.write.GetStages() | current_suffix.read_accesses.GetStages();

            barrier.oldLayout = current_suffix.layout;
            barrier.newLayout = prefix.layout;

            for (const auto& [access, stages] : prefix.read_accesses.GetReaders()) {
                barrier.dstAccessMask |= access;
                barrier.dstStageMask |= stages;
            }
            barrier.dstAccessMask |= prefix.write.GetAccess();
            barrier.dstStageMask |= prefix.write.GetStages();

            res.push_back(barrier);
        }

        return res;
    }

    void UpdateImageState(IImage& image, const ImageTracker::State& prefix, const std::optional<ImageTracker::State>& suffix) {
        auto& current_suffix = image.GetSyncState();

        if (suffix.has_value()) {
            current_suffix = *suffix;
        } else {
            if (prefix.layout == current_suffix.layout) {
                for (const auto& [access, stages] : prefix.read_accesses.GetReaders()) {
                    current_suffix.read_accesses.AddAccess({stages, access});
                }
            } else {
                current_suffix = prefix;
            }
        }
    }

}  // namespace Engine::Vulkan::Synchronization