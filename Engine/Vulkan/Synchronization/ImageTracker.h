#pragma once

#include "AccessScope.h"

namespace Engine::Vulkan::Synchronization {

    class ImageTracker {
    public:
        class ReadAccesses {
        public:
            using Readers = std::unordered_map<VkAccessFlags2, VkPipelineStageFlags2>;

            VkPipelineStageFlags2 GetStages() const;
            const Readers&        GetReaders() const;

            std::pair<VkAccessFlags2, VkPipelineStageFlags2> AddAccess(const AccessScope& scope);

        private:
            Readers               m_Readers;
            VkPipelineStageFlags2 m_Stages = VK_PIPELINE_STAGE_2_NONE;
        };

        struct State {
            VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
            ReadAccesses  read_accesses;
            AccessScope   write;
        };

    public:
        std::optional<VkImageMemoryBarrier2> AccessRequest(AccessScope scope, VkImageLayout layout);

        const State&                GetPrefix() const;
        const std::optional<State>& GetSuffix() const;

    private:
        VkImageLayout GetCurrentLayout() const;

    private:
        State                m_Prefix;
        std::optional<State> m_Suffix;
    };

    std::vector<VkImageMemoryBarrier2> ConnectSyncStates(const IImage& image, const ImageTracker::State& suffix, const ImageTracker::State& prefix);
    void UpdateImageState(IImage& image, const ImageTracker::State& prefix, const std::optional<ImageTracker::State>& suffix);

}  // namespace Engine::Vulkan::Synchronization