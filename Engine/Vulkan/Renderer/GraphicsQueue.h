#pragma once

#include "Engine/Vulkan/Renderer/Queue.h"

namespace Engine::Vulkan {
    class GraphicsQueue : public Queue {
    public:
        GraphicsQueue(Device* device, VkQueue queue, uint32_t queue_family_index);
        ~GraphicsQueue() = default;
    };
}  // namespace Engine::Vulkan