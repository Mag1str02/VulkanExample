#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Interface {

    class RawCommandBuffer {
    public:
        virtual ~RawCommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End()   = 0;
        virtual void Reset() = 0;

        virtual void AddImageMemoryBarrier(const VkImageMemoryBarrier2& barier)   = 0;
        virtual void AddBufferMemoryBarrier(const VkBufferMemoryBarrier2& barier) = 0;
        virtual void FlushBariers()                                               = 0;

        virtual void BeginRendering(const std::vector<Ref<ImageView>>& color_attachments) = 0;
        virtual void EndRendering()                                                       = 0;

        virtual void ClearImage(IImage& image, Vec4 clear_color)                       = 0;
        virtual void ExecuteCommandBuffer(IRawCommandBuffer& secondary_command_buffer) = 0;

        virtual const VkCommandBuffer& Handle() const      = 0;
        virtual const bool             IsSecondary() const = 0;
    };

};  // namespace Engine::Vulkan::Interface