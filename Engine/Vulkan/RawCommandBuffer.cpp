#include "RawCommandBuffer.h"

#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan {

    Ref<RawCommandBuffer> RawCommandBuffer::Create(Ref<CommandPool> pool, bool secondary) {
        return Ref<RawCommandBuffer>(new RawCommandBuffer(pool, secondary));
    }
    RawCommandBuffer::RawCommandBuffer(Ref<CommandPool> pool, bool secondary) : m_Pool(pool) {
        PROFILER_SCOPE("Engine::Vulkan::RawCommandBuffer::RawCommandBuffer");
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = m_Pool->Handle();
        allocInfo.level              = (secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer handle;
        VK_CHECK(vkAllocateCommandBuffers(m_Pool->GetQueue()->GetDevice()->GetLogicDevice(), &allocInfo, &handle));

        Init(handle, secondary);
    }

    RawCommandBuffer::~RawCommandBuffer() {
        PROFILER_SCOPE("Engine::Vulkan::RawCommandBuffer::~RawCommandBuffer");
        vkFreeCommandBuffers(m_Pool->GetQueue()->GetDevice()->GetLogicDevice(), m_Pool->Handle(), 1, &Handle());
    }

}  // namespace Engine::Vulkan