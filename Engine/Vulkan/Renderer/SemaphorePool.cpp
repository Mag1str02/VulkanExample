#include "SemaphorePool.h"

#include "Device.h"

namespace Engine::Vulkan {

    Semaphore::Semaphore(Ref<SemaphorePool> pool, VkSemaphore handle) : m_Handle(handle), m_Pool(pool) {}
    Semaphore::~Semaphore() {
        if (m_Handle != VK_NULL_HANDLE) {
            m_Pool->m_Semaphores.push_back(m_Handle);
        }
    }

    VkSemaphore Semaphore::Handle() {
        return m_Handle;
    }

    Ref<SemaphorePool> SemaphorePool::Create(Ref<Device> device) {
        return Ref<SemaphorePool>(new SemaphorePool(device));
    }
    SemaphorePool::~SemaphorePool() {
        m_Device->WaitIdle();
        for (const auto& semaphore : m_Semaphores) {
            vkDestroySemaphore(m_Device->GetLogicDevice(), semaphore, nullptr);
        }
    }

    SemaphorePool::SemaphorePool(Ref<Device> device) : m_Device(device) {}
    Ref<Semaphore> SemaphorePool::CreateSemaphore() {
        if (m_Semaphores.empty()) {
            VkSemaphore           semaphore;
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VK_CHECK(vkCreateSemaphore(m_Device->GetLogicDevice(), &info, nullptr, &semaphore));
            m_Semaphores.push_back(semaphore);
        }
        VkSemaphore semaphore = m_Semaphores.front();
        m_Semaphores.pop_front();
        return Ref<Semaphore>(new Semaphore(shared_from_this(), semaphore));
    }

}  // namespace Engine::Vulkan