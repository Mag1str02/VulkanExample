#include "AccessScope.h"

namespace Engine::Vulkan::Synchronization {

    namespace {
        static constexpr VkAccessFlags2 kWriteAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT;
        static constexpr VkAccessFlags2 kReadAccessMask  = 0;
    }  // namespace

    AccessScope::AccessScope(VkPipelineStageFlags2 stages, VkAccessFlags2 access) : m_Stage(stages), m_Access(access) {
        Verify();
    }

    void AccessScope::Verify() const {
        VkAccessFlags2 supported_accesses_mask = kWriteAccessMask | kReadAccessMask;
        DE_ASSERT((~supported_accesses_mask & m_Access) == 0, std::format("Found unsupported access mask: {:x}", m_Access));
        DE_ASSERT(std::popcount(GetReadAccess()) <= 1, "Cannot use multiple read access in single access request");
        DE_ASSERT(std::popcount(GetWriteAccess()) <= 1, "Cannot use multiple write access in single access request");
        DE_ASSERT(std::popcount(m_Access) > 0, "Cannot create access scope without access mask");
        DE_ASSERT(std::popcount(m_Stage) > 0, "Cannot create access scope without stage mask");

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

}  // namespace Engine::Vulkan::Synchronization