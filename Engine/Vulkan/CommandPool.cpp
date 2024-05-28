#include "CommandPool.h"

#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan {

    Ref<CommandPool> CommandPool::Create(Ref<Queue> queue, bool transient) {
        return Ref<CommandPool>(new CommandPool(std::move(queue), transient));
    }
    CommandPool::CommandPool(Ref<Queue> queue, bool transient) : m_Queue(std::move(queue)) {
        PROFILER_SCOPE("Engine::Vulkan::CommandPool::CommandPool");

        VkCommandPool           handle;
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_Queue->FamilyIndex();
        if (transient) {
            poolInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        }

        VK_CHECK(vkCreateCommandPool(m_Queue->GetDevice()->GetLogicDevice(), &poolInfo, nullptr, &handle));

        Init(m_Queue.get(), handle);
    }

    CommandPool::~CommandPool() {
        PROFILER_SCOPE("Engine::Vulkan::CommandPool::~CommandPool");
        vkDestroyCommandPool(m_Queue->GetDevice()->GetLogicDevice(), Handle(), nullptr);
    }

}  // namespace Engine::Vulkan