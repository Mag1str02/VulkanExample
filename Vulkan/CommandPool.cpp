#include "CommandPool.h"

#include "Vulkan/CommandBuffer.h"
#include "Vulkan/Device.h"

namespace Vulkan {

    CommandPool::CommandPool(Ref<Device> device, uint32_t familyIndex) {
        DE_ASSERT(device, "Bad device");
        m_Device      = device;
        m_FamilyIndex = familyIndex;

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_FamilyIndex;
        VK_CHECK(vkCreateCommandPool(m_Device->GetLogicDevice(), &poolInfo, nullptr, &m_Handle));
    }

    CommandPool::~CommandPool() {
        vkDestroyCommandPool(m_Device->GetLogicDevice(), m_Handle, nullptr);
    }

    const VkCommandPool& CommandPool::Handle() {
        return m_Handle;
    }
    uint32_t CommandPool::FamilyIndex() {
        return m_FamilyIndex;
    }

    Ref<CommandBuffer> CommandPool::CreateCommandBuffer() {
        return Ref<CommandBuffer>(new CommandBuffer(shared_from_this()));
    }

    Ref<Device> CommandPool::GetDevice() {
        return m_Device;
    }

}  // namespace Vulkan