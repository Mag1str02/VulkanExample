#pragma once

#include "Object.h"

namespace Engine::Vulkan {
    class Queue {
    public:
        Queue(VkQueue queue, uint32_t queue_family_index);

        VkQueue  Handle();
        uint32_t FamilyIndex();

    private:
        VkQueue  m_Queue;
        uint32_t m_FamilyIndex;
    };
}  // namespace Engine::Vulkan