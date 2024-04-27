#include "State.h"

#if 0
#define LOG(...) std::println(__VA_ARGS__)
#else
#define LOG(...)
#endif

namespace Engine::Vulkan::Synchronization {

    namespace {
        static constexpr VkAccessFlags2          kWriteAccessMask           = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT;
        static constexpr VkAccessFlags2          kReadAccessMask            = 0;
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

    AccessScope::AccessScope(VkPipelineStageFlags2 stages, VkAccessFlags2 access) : m_Stage(stages), m_Access(access) {
        Verify();
    }

    void AccessScope::Verify() const {
        VkAccessFlags2 supported_accesses_mask = kWriteAccessMask | kReadAccessMask;
        DE_ASSERT((~supported_accesses_mask & m_Access) == 0, std::format("Found unsupported access mask: {:x}", m_Access));
        DE_ASSERT(std::popcount(GetReadAccess()) <= 1, "Cannot use multiple read access in single access request");
        DE_ASSERT(std::popcount(GetWriteAccess()) <= 1, "Cannot use multiple write access in single access request");

        // TODO: Check stage-access match
    }
    bool AccessScope::Empty() const {
        return m_Access == VK_ACCESS_2_NONE && m_Stage == VK_PIPELINE_STAGE_2_NONE;
    }
    VkAccessFlagBits2 AccessScope::GetReadAccess() const {
        return m_Access & kReadAccessMask;
    }
    VkAccessFlagBits2 AccessScope::GetWriteAccess() const {
        return m_Access & kWriteAccessMask;
    }
    VkAccessFlags2 AccessScope::GetAccess() const {
        return m_Access;
    }
    VkPipelineStageFlags2 AccessScope::GetStages() const {
        return m_Stage;
    }

    std::vector<VkImageMemoryBarrier2> ImageState::AccessRequest(AccessScope scope, VkImageLayout layout) {
        if (m_LastLayout == VK_IMAGE_LAYOUT_MAX_ENUM) {
            m_LastLayout    = layout;
            m_InitialLayout = layout;
        }
        std::vector<VkImageMemoryBarrier2> barriers;
        LOG("Access Request: Stages={:x} Access={:x} Layout={}", scope.GetStages(), scope.GetAccess(), (uint32_t)layout);

        {
            bool emmit   = false;
            auto barrier = CreateImageBarrier();

            if (!m_LastUnavailableWrite.Empty()) {
                barrier.srcAccessMask = m_LastUnavailableWrite.GetAccess();
                barrier.srcStageMask  = m_LastUnavailableWrite.GetStages();
                emmit                 = true;

                m_LastUnavailableWrite = AccessScope();

                LOG("Emmiting last write availability...");
            }

            if (layout != m_InitialLayout) {
                barrier.oldLayout = m_LastLayout;
                barrier.newLayout = layout;
                emmit             = true;

                m_LastLayout = layout;
                m_VisibleStages.clear();  // Assuming that layout transition makes execution dependency with all operation before

                LOG("Emmiting layout transition...");
            } else if (scope.GetWriteAccess() != VK_ACCESS_2_NONE && !m_VisibleStages.empty()) {
                auto barrier = CreateImageBarrier();
                for (const auto& [access, stages] : m_VisibleStages) {
                    barrier.srcStageMask |= stages;
                }
                barrier.dstStageMask = scope.GetStages();
                barriers.push_back(barrier);

                m_VisibleStages.clear();

                LOG("Emmiting execution dependency for WAR...");
            }

            // Visibility operations for access scope
            {
                if (scope.GetReadAccess() != VK_ACCESS_2_NONE) {
                    auto it = m_VisibleStages.find(scope.GetReadAccess());
                    if (it == m_VisibleStages.end() || (it->second & scope.GetStages()) != scope.GetStages()) {
                        it = m_VisibleStages.try_emplace(scope.GetReadAccess(), VK_PIPELINE_STAGE_2_NONE).first;
                        barrier.dstAccessMask |= scope.GetReadAccess();
                        barrier.dstStageMask |= (it->second & scope.GetStages()) ^ scope.GetStages();
                        it->second |= scope.GetStages();
                        emmit = true;

                        LOG("Emmiting visibility for read access...");
                    }
                }
                if (scope.GetWriteAccess() != VK_ACCESS_2_NONE) {
                    auto it = m_VisibleStages.find(scope.GetWriteAccess());
                    if (it == m_VisibleStages.end() || (it->second & scope.GetStages()) != scope.GetStages()) {
                        it = m_VisibleStages.try_emplace(scope.GetWriteAccess(), VK_PIPELINE_STAGE_2_NONE).first;
                        barrier.dstAccessMask |= scope.GetWriteAccess();
                        barrier.dstStageMask |= (it->second & scope.GetStages()) ^ scope.GetStages();
                        it->second |= scope.GetStages();
                        emmit = true;

                        LOG("Emmiting visibility for write access...");
                    }
                }
            }
            if (emmit) {
                barriers.push_back(barrier);
            }
        }

        LOG("Access Request Complete: {} barriers emmited\n", barriers.size());
        return barriers;
    }

    VkImageLayout ImageState::GetInitialImageLayout() const {
        return m_InitialLayout;
    }
    VkPipelineStageFlags2 ImageState::GetFirstWriteStages() const {
        return m_FirstWriteStages;
    }
}  // namespace Engine::Vulkan::Synchronization