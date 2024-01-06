#pragma once

#include "Vulkan/Common.h"

namespace Vulkan {

    class CommandBuffer {
    public:
        ~CommandBuffer();

        void Begin();
        void End();
        void Reset();
        void SetViewport(float width, float height, float xOffset = 0, float yOffset = 0, float minDepth = 0, float maxDepth = 1);
        void SetScissor(uint32_t width, uint32_t height, int32_t xOffset = 0, int32_t yOffset = 0);
        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);

        const VkCommandBuffer& Handle();

        NO_COPY_CONSTRUCTORS(CommandBuffer);
        NO_MOVE_CONSTRUCTORS(CommandBuffer);

    private:
        CommandBuffer(Ref<CommandPool> pool);

    private:
        friend class CommandPool;

        VkCommandBuffer  m_Handle;
        Ref<CommandPool> m_Pool;
    };
}  // namespace Vulkan