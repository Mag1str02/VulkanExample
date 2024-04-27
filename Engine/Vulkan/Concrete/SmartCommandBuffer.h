#pragma once

#include "Engine/Vulkan/Common.h"

#include "Engine/Vulkan/Synchronization/Tracker.h"

namespace Engine::Vulkan::Concrete {
    class SmartCommandBuffer {
    public:
        static Ref<SmartCommandBuffer> Create(Ref<CommandPool> pool);

        void Begin();
        void End();

        void BeginRendering(const std::vector<Ref<ImageView>>& color_attachments);
        void EndRendering();
        void ClearImage(Ref<IImage> image, Vec4 clear_color);

        void PreparePresent(Ref<IImage> image);
        void PrepareColorAttachment(Ref<IImage> image);
        void PrepareClear(Ref<IImage> image);

        void HintInitialLayout(Ref<IImage> image, VkImageLayout layout);

        void AddImageMemoryBarrier(const VkImageMemoryBarrier2& barrier);  // TMP

        VkCommandBuffer Handle();

    private:
        SmartCommandBuffer(Ref<CommandPool> pool);
        void RequestImageAccess(Ref<IImage> image, Synchronization::AccessScope scope, VkImageLayout layout);

    private:
        Ref<CommandBuffer>       m_CommandBuffer;
        Synchronization::Tracker m_Tracker;
    };
}  // namespace Engine::Vulkan::Concrete