#include "State.h"
#include <optional>
#include "vulkan/vulkan_core.h"

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

    std::optional<VkImageMemoryBarrier2> ImageState::AccessRequest(AccessScope scope, VkImageLayout layout) {
        DE_ASSERT(layout != VK_IMAGE_LAYOUT_UNDEFINED, "Access with image layout undefined is not supported");
        PROFILER_SCOPE("Engine::Vulkan::Synchronization::ImageState::AccessRequest");

        if (m_InitialLayout == VK_IMAGE_LAYOUT_MAX_ENUM) {
            m_InitialLayout = layout;
            m_CurrentLayout = layout;
        }

        bool is_write = scope.GetWriteAccess() != 0 || m_CurrentLayout != layout;

        std::optional<VkImageMemoryBarrier2> result;
        VkImageMemoryBarrier2                barrier = CreateImageBarrier();
        barrier.oldLayout                            = m_CurrentLayout;
        barrier.newLayout                            = layout;
        if (m_WasFirstWrite) {
            if (is_write) {
                barrier.srcAccessMask = m_LastWriteAccess;  // TODO: Maybe access op should only be performed once
                barrier.srcStageMask  = m_LastWritePipelineStages | m_SynchronizedReadersStages;

                barrier.dstAccessMask = scope.GetAccess();
                barrier.dstStageMask  = scope.GetStages();

                result = barrier;

                m_SynchronizedReadersStages = VK_PIPELINE_STAGE_2_NONE;
                m_SynchronizedReaders.clear();

                m_LastWriteAccess         = scope.GetWriteAccess();
                m_LastWritePipelineStages = scope.GetStages();
            } else {
                VkAccessFlags2        access = VK_ACCESS_2_NONE;
                VkPipelineStageFlags2 stages = VK_PIPELINE_STAGE_2_NONE;

                auto it = m_SynchronizedReaders.find(scope.GetAccess());
                if (it == m_SynchronizedReaders.end()) {
                    access = scope.GetAccess();  // TODO: maybe visibility operations should only be performed once
                    stages = scope.GetStages();

                    m_SynchronizedReadersStages |= stages;
                    m_SynchronizedReaders[scope.GetAccess()] = scope.GetStages();
                } else if ((it->second & scope.GetStages()) != scope.GetStages()) {
                    access = scope.GetAccess();  // TODO: maybe visibility operations should only be performed once
                    stages = (it->second & scope.GetStages()) ^ scope.GetStages();

                    m_SynchronizedReadersStages |= stages;
                    it->second |= scope.GetStages();
                }

                if (stages != VK_PIPELINE_STAGE_2_NONE || access != VK_ACCESS_2_NONE) {
                    barrier.srcAccessMask = m_LastWriteAccess;
                    barrier.srcStageMask  = m_LastWritePipelineStages;

                    barrier.dstAccessMask = access;
                    barrier.dstStageMask  = stages;

                    result = barrier;
                }
            }
        } else {
            if (is_write) {
                barrier.srcStageMask = m_SynchronizedReadersStages;

                barrier.dstStageMask = scope.GetStages();

                result = barrier;

                m_LastWriteAccess         = scope.GetWriteAccess();
                m_LastWritePipelineStages = scope.GetStages();

                m_InitialWriteAccess         = scope.GetAccess();
                m_InitialWritePipelineStages = scope.GetStages();

                m_InitialSynchronizedReadersStages = m_SynchronizedReadersStages;
                m_InitialSynchronizedReaders       = m_SynchronizedReaders;
                m_WasFirstWrite                    = true;

                m_SynchronizedReadersStages = VK_PIPELINE_STAGE_2_NONE;
                m_SynchronizedReaders.clear();
            } else {
                m_SynchronizedReadersStages |= scope.GetStages();
                m_SynchronizedReaders[scope.GetAccess()] |= scope.GetStages();
            }
        }

        m_CurrentLayout = layout;
        return result;
    }

}  // namespace Engine::Vulkan::Synchronization