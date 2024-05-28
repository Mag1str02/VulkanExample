#pragma once

#include "AccessScope.h"

namespace Engine::Vulkan::Synchronization {

    class ImageTracker {
    public:
        class ReadAccesses {
        public:
            using Readers = std::unordered_map<VkAccessFlags2, VkPipelineStageFlags2>;

            VkPipelineStageFlags2 GetStages() const;
            VkAccessFlags2        GetAccess() const;
            const Readers&        GetReaders() const;

            std::pair<VkAccessFlags2, VkPipelineStageFlags2> AddAccess(const AccessScope& scope);

        private:
            Readers               m_Readers;
            VkPipelineStageFlags2 m_Stages = VK_PIPELINE_STAGE_2_NONE;
            VkAccessFlags2        m_Access = VK_ACCESS_2_NONE;
        };

        struct State {
            VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
            ReadAccesses  read_accesses;
            AccessScope   write;

            bool HasWrite() const;
            bool Empty() const;
        };

    public:
        std::optional<VkImageMemoryBarrier2> AccessRequest(AccessScope scope, VkImageLayout layout);

        const State& GetPrefix() const;
        const State& GetSuffix() const;

    private:
        State m_Prefix;
        State m_Suffix;
    };

}  // namespace Engine::Vulkan::Synchronization