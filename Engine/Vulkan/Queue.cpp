#include "Queue.h"

#include "Device.h"

#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Concrete/Fence.h"
#include "Engine/Vulkan/Interface/Fence.h"

namespace Engine::Vulkan {

    Queue::Queue(Device* device, VkQueue queue, uint32_t queue_family_index) : m_Device(device), m_Queue(queue), m_FamilyIndex(queue_family_index) {}
    Queue::~Queue() {
        WaitIdle();
        RemoveCompleted();
        DE_ASSERT(m_RunningTasks.empty(), "Unfinished tasks");
    }

    Ref<CommandPool> Queue::GetPool() {
        if (m_Pool == nullptr) {
            m_Pool = CommandPool::Create(m_Device->shared_from_this(), m_FamilyIndex);
        }
        return m_Pool;
    }

    void Queue::WaitIdle() {
        PROFILER_SCOPE("Engine::Vulkan::Queue::WaitIdle");
        VK_CHECK(vkQueueWaitIdle(m_Queue));
    }
    void Queue::Submit(Ref<Task> task) {
        PROFILER_SCOPE("Engine::Vulkan::Queue::Submit");

        std::println("Submiting task...");

        TaskContext ctx;
        ctx.task = task;
        if (task->RequiresBarriers()) {
            ctx.secondary_fence = CreateRef<Concrete::Fence>(m_Device->shared_from_this());

            auto secondary_command_buffer = Concrete::CommandBuffer::Create(GetPool());

            secondary_command_buffer->Begin();
            task->RecordBarriers(*secondary_command_buffer);
            secondary_command_buffer->End();

            VkSubmitInfo info{};
            info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.commandBufferCount = 1;
            info.pCommandBuffers    = &secondary_command_buffer->Handle();

            std::println("Submiting secondary command buffer...");
            VK_CHECK(vkQueueSubmit(m_Queue, 1, &info, ctx.secondary_fence->Handle()));

            ctx.syncronization_buffer = std::move(secondary_command_buffer);
        }
        std::println("Submiting main task");
        if (task->RequiresSemaphore()) {
            auto new_semaphore = CreateSemaphore();
            task->Run(m_Queue, latest_semaphore, new_semaphore);
            ctx.wait_semaphore = latest_semaphore;
            latest_semaphore   = new_semaphore;
        } else {
            task->Run(m_Queue, VK_NULL_HANDLE, VK_NULL_HANDLE);
        }

        m_RunningTasks.emplace_back(std::move(ctx));

        RemoveCompleted();
    }

    VkSemaphore Queue::CreateSemaphore() {
        VkSemaphore           semaphore;
        VkSemaphoreCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK(vkCreateSemaphore(m_Device->GetLogicDevice(), &info, nullptr, &semaphore));
        return semaphore;
    }
    void Queue::DestroySemaphore(VkSemaphore semaphore) {
        if (semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(m_Device->GetLogicDevice(), semaphore, nullptr);
        }
    }

    void Queue::RemoveCompleted() {
        for (size_t i = 0; i < m_RunningTasks.size(); ++i) {
            auto& ctx = m_RunningTasks[i];
            if (ctx.task->IsCompleted() && (ctx.secondary_fence == nullptr || ctx.secondary_fence->IsSignaled())) {
                DestroySemaphore(m_RunningTasks[i].wait_semaphore);
                m_RunningTasks.erase(m_RunningTasks.begin() + i);
                --i;
            }
        }
    }

    VkQueue Queue::Handle() {
        return m_Queue;
    }
    uint32_t Queue::FamilyIndex() {
        return m_FamilyIndex;
    }
    Ref<Device> Queue::GetDevice() {
        return Ref<Device>(m_Device->shared_from_this(), m_Device);
    }

}  // namespace Engine::Vulkan