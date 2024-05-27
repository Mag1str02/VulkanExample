#include "BinarySemaphorePool.h"

#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan {

    BinarySemaphorePool::BinarySemaphore::BinarySemaphore(Ref<BinarySemaphorePool> pool, VkSemaphore handle) :
        m_Handle(handle), m_Pool(std::move(pool)) {}
    BinarySemaphorePool::BinarySemaphore::~BinarySemaphore() {
        m_Pool->m_Semaphores.push_back(m_Handle);
    }

    const VkSemaphore& BinarySemaphorePool::BinarySemaphore::Handle() const {
        return m_Handle;
    }

    Ref<BinarySemaphorePool> BinarySemaphorePool::Create(Ref<Device> device) {
        return Ref<BinarySemaphorePool>(new BinarySemaphorePool(device));
    }
    BinarySemaphorePool::~BinarySemaphorePool() {
        m_Device->WaitIdle();
        for (const auto& semaphore : m_Semaphores) {
            vkDestroySemaphore(m_Device->GetLogicDevice(), semaphore, nullptr);
        }
    }

    BinarySemaphorePool::BinarySemaphorePool(Ref<Device> device) : m_Device(device) {}
    Ref<IBinarySemaphore> BinarySemaphorePool::CreateSemaphore() {
        if (m_Semaphores.empty()) {
            VkSemaphore           semaphore;
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VK_CHECK(vkCreateSemaphore(m_Device->GetLogicDevice(), &info, nullptr, &semaphore));
            m_Semaphores.push_back(semaphore);
        }
        VkSemaphore semaphore = m_Semaphores.front();
        m_Semaphores.pop_front();
        return Ref<BinarySemaphore>(new BinarySemaphore(shared_from_this(), semaphore));
    }
    Ref<Device> BinarySemaphorePool::GetDevice() const {
        return m_Device;
    }

}  // namespace Engine::Vulkan