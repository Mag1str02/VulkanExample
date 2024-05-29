#pragma once

#include "RawCommandBuffer.h"

#include "Engine/Vulkan/Common.h"

#include "Engine/Vulkan/Synchronization/Tracker.h"

namespace Engine::Vulkan {

    class CommandBuffer {
    public:
        static Ref<CommandBuffer> Create(Ref<CommandPool> pool);

        void Begin();
        void End();

        void BeginRendering(const std::vector<Ref<ImageView>>& color_attachments);
        void EndRendering();

        void ClearImage(Ref<IImage> image, Vec4 clear_color);

        void RequestImageAccess(Ref<IImage> image, Synchronization::AccessScope scope, VkImageLayout layout);

        const IRawCommandBuffer&        GetRawCommandBuffer() const;
        const Synchronization::Tracker& GetSynchronizationTracker() const;

    private:
        CommandBuffer(Ref<CommandPool> pool);

        void PreparePresent(Ref<IImage> image);
        void PrepareColorAttachment(Ref<IImage> image);
        void PrepareClear(Ref<IImage> image);

    private:
        friend class Executor;

        Ref<CommandPool>                   m_Pool;
        Ref<RawCommandBuffer>              m_MainCommandBuffer;
        std::vector<Ref<RawCommandBuffer>> m_SecondaryCommandBuffers;

        RawCommandBuffer* m_CurrentCommandBuffer;

        Synchronization::Tracker m_Tracker;
    };
}  // namespace Engine::Vulkan