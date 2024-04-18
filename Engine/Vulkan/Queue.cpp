#include "Queue.h"

namespace Engine::Vulkan {
    Queue::Queue(VkQueue queue, uint32_t queue_family_index) : m_Queue(queue), m_FamilyIndex(queue_family_index) {}

    VkQueue Queue::Handle() {
        return m_Queue;
    }
    uint32_t Queue::FamilyIndex() {
        return m_FamilyIndex;
    }

}  // namespace Engine::Vulkan