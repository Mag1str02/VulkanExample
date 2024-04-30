#pragma once

#include "Engine/Vulkan/Common.h"
#include "vulkan/vulkan_core.h"

namespace Engine::Vulkan::Synchronization {
    class AccessScope {
    public:
        AccessScope() = default;
        AccessScope(VkPipelineStageFlags2 stages, VkAccessFlags2 access);

        bool           Empty() const;
        VkAccessFlags2 GetReadAccess() const;
        VkAccessFlags2 GetWriteAccess() const;

        VkAccessFlags2        GetAccess() const;
        VkPipelineStageFlags2 GetStages() const;

    private:
        void Verify() const;

    private:
        VkPipelineStageFlags2 m_Stage  = VK_PIPELINE_STAGE_2_NONE;
        VkAccessFlags2        m_Access = VK_ACCESS_2_NONE;
    };

    class ImageState {
    public:
        std::optional<VkImageMemoryBarrier2> AccessRequest(AccessScope scope, VkImageLayout layout);

    private:
        using SynchronizedReaders = std::unordered_map<VkAccessFlags2, VkPipelineStageFlags2>;

        bool          m_WasFirstWrite = true;
        VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

        VkAccessFlags2        m_LastWriteAccess         = VK_ACCESS_2_NONE;
        VkPipelineStageFlags2 m_LastWritePipelineStages = VK_PIPELINE_STAGE_2_NONE;

        VkPipelineStageFlags2 m_SynchronizedReadersStages = VK_ACCESS_2_NONE;
        SynchronizedReaders   m_SynchronizedReaders;

        VkImageLayout m_InitialLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

        VkAccessFlags2        m_InitialWriteAccess         = VK_ACCESS_2_NONE;
        VkPipelineStageFlags2 m_InitialWritePipelineStages = VK_PIPELINE_STAGE_2_NONE;

        VkPipelineStageFlags2 m_InitialSynchronizedReadersStages = VK_PIPELINE_STAGE_2_NONE;
        SynchronizedReaders   m_InitialSynchronizedReaders;
    };
}  // namespace Engine::Vulkan::Synchronization