#pragma once

#include "Engine/Vulkan/Renderer/Queue.h"
#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan {
    class PresentationQueue final : public Queue {
    public:
        PresentationQueue(Device* device, VkQueue queue, uint32_t queue_family_index);
        ~PresentationQueue() = default;

        VkResult Present(Ref<SwapChain::Image> image, VkSemaphore wait_semaphore);
    };
}  // namespace Engine::Vulkan