#pragma once

#include "Engine/Vulkan/Common.h"

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
        std::vector<VkImageMemoryBarrier2> AccessRequest(AccessScope scope, VkImageLayout layout);

        VkImageLayout         GetInitialImageLayout() const;
        VkPipelineStageFlags2 GetFirstWriteStages() const;

    private:
        AccessScope   m_LastUnavailableWrite;
        VkImageLayout m_LastLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

        std::unordered_map<VkAccessFlags2, VkPipelineStageFlags2> m_VisibleStages;

        VkImageLayout         m_InitialLayout    = VK_IMAGE_LAYOUT_MAX_ENUM;
        VkPipelineStageFlags2 m_FirstWriteStages = VK_PIPELINE_STAGE_2_NONE;
    };
}  // namespace Engine::Vulkan::Synchronization