#include "CommandBuffer.h"

#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Device.h"

namespace Engine::Vulkan::Concrete {

    Ref<CommandBuffer> CommandBuffer::Create(Ref<CommandPool> pool) {
        return Ref<CommandBuffer>(new CommandBuffer(pool));
    }
    CommandBuffer::CommandBuffer(Ref<CommandPool> pool) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = pool->Handle();
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer handle;
        VK_CHECK(vkAllocateCommandBuffers(pool->GetDevice()->GetLogicDevice(), &allocInfo, &handle));

        Init(handle, pool);
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
        ResetSecondaryCommandBuffers();
        VK_CHECK(vkResetCommandBuffer(m_Handle, 0));
    }

}  // namespace Engine::Vulkan::Concrete