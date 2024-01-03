#include "Queue.h"

#include "Device.h"

namespace Vulkan {

    VkQueue Queue::Handle() {
        return m_Handle;
    }

    Queue::Queue(Ref<Device> device, VkQueue handle) {
        DE_ASSERT(handle != VK_NULL_HANDLE, "Bad handle");
        DE_ASSERT(device, "Bad device");

        m_Device = device;
        m_Handle = handle;
    }
}  // namespace Vulkan