#include "Queue.h"

#include "Device.h"

namespace Engine::Vulkan {

    const VkQueue& Queue::Handle() {
        DE_ASSERT(m_Valid, "Invalid queue");
        return m_Handle;
    }

    uint32_t Queue::FamilyIndex() const {
        DE_ASSERT(m_Valid, "Invalid queue");
        return m_FamilyIndex;
    }

    void Queue::Invalidate() {
        m_Valid = false;
    }

    Queue::Queue(VkQueue handle, uint32_t familyIndex) : m_FamilyIndex(familyIndex), m_Handle(handle) {
        DE_ASSERT(handle != VK_NULL_HANDLE, "Bad handle");
    }
}  // namespace Vulkan