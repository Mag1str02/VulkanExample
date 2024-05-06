#include "Semaphore.h"

#include "Engine/Vulkan/Device.h"

namespace Engine::Vulkan::Concrete {
    Semaphore::Semaphore(Ref<Device> device) {
        VkSemaphore           semaphore;
        VkSemaphoreCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VK_CHECK(vkCreateSemaphore(device->GetLogicDevice(), &info, nullptr, &semaphore));

        Init(device, semaphore);
    }
    Semaphore::~Semaphore() {
        vkDestroySemaphore(m_Device->GetLogicDevice(), m_Handle, nullptr);
    }

}  // namespace Engine::Vulkan::Concrete