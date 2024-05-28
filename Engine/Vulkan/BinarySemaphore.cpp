#include "BinarySemaphore.h"

#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan {

    Ref<BinarySemaphore> BinarySemaphore::Create(Ref<Device> device) {
        return Ref<BinarySemaphore>(new BinarySemaphore(std::move(device)));
    }

    BinarySemaphore::BinarySemaphore(Ref<Device> device) : m_Device(std::move(device)) {
        VkSemaphoreCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK(vkCreateSemaphore(m_Device->GetLogicDevice(), &info, nullptr, &m_Handle));
    }
    BinarySemaphore::~BinarySemaphore() {
        vkDestroySemaphore(m_Device->GetLogicDevice(), m_Handle, nullptr);
    }

    const VkSemaphore& BinarySemaphore::Handle() const {
        return m_Handle;
    }

}  // namespace Engine::Vulkan