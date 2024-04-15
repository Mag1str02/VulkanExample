#include "CommandBuffer.h"

#include "CommandPool.h"
#include "Device.h"

namespace Engine::Vulkan {

    CommandBuffer::CommandBuffer(Ref<CommandPool> pool) {
        DE_ASSERT(pool, "Bad pool");
        m_Pool = pool;

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = m_Pool->Handle();
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(m_Pool->GetDevice()->GetLogicDevice(), &allocInfo, &m_Handle));
    }

    CommandBuffer::~CommandBuffer() {
        vkFreeCommandBuffers(m_Pool->GetDevice()->GetLogicDevice(), m_Pool->Handle(), 1, &m_Handle);
    }

    void CommandBuffer::Begin() {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(vkBeginCommandBuffer(m_Handle, &beginInfo));
    }
    void CommandBuffer::End() {
        VK_CHECK(vkEndCommandBuffer(m_Handle));
    }
    void CommandBuffer::Reset() {
        VK_CHECK(vkResetCommandBuffer(m_Handle, 0));
    }
    void CommandBuffer::SetViewport(float width, float height, float xOffset, float yOffset, float minDepth, float maxDepth) {
        VkViewport viewport{};
        viewport.x        = xOffset;
        viewport.y        = yOffset;
        viewport.width    = width;
        viewport.height   = height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        vkCmdSetViewport(m_Handle, 0, 1, &viewport);
    }
    void CommandBuffer::SetScissor(uint32_t width, uint32_t height, int32_t xOffset, int32_t yOffset) {
        VkRect2D scissor{};
        scissor.offset.x      = xOffset;
        scissor.offset.y      = yOffset;
        scissor.extent.width  = width;
        scissor.extent.height = height;
        vkCmdSetScissor(m_Handle, 0, 1, &scissor);
    }
    void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
        vkCmdDraw(m_Handle, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    const VkCommandBuffer& CommandBuffer::Handle() {
        return m_Handle;
    }

}  // namespace Engine::Vulkan