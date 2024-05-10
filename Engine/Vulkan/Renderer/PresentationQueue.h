#pragma once

#include "Engine/Vulkan/Renderer/Queue.h"

namespace Engine::Vulkan {
    class PresentationQueue final : public Queue {
    public:
        PresentationQueue(Device* device, VkQueue queue, uint32_t queue_family_index);
        ~PresentationQueue() = default;
    };
}  // namespace Engine::Vulkan