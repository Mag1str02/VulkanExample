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

}  // namespace Engine::Vulkan::Synchronization