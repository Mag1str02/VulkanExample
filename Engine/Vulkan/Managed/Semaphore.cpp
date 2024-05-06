#include "Semaphore.h"

namespace Engine::Vulkan::Managed {

    void Semaphore::Init(Ref<Device> device, VkSemaphore Semaphore) {
        m_Device = device;
        m_Handle = Semaphore;
    }

    VkSemaphore Semaphore::Handle() {
        return m_Handle;
    }

}  // namespace Engine::Vulkan::Managed