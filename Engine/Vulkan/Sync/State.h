#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Sync {
    struct Access {
        VkPipelineStageFlags2 m_Stage  = VK_PIPELINE_STAGE_2_NONE;
        VkAccessFlags2        m_Access = VK_ACCESS_2_NONE;
    };

    struct State {
        Access                                                    m_LastWrite;
        std::unordered_map<VkAccessFlags2, VkPipelineStageFlags2> m_SynchonizedReaders;
    };

    struct ImageState : State {
        VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
    };
}  // namespace Engine::Vulkan::Sync