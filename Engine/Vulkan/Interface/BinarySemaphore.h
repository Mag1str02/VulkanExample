#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Interface {

    class BinarySemaphore {
    public:
        virtual ~BinarySemaphore() = default;

        virtual const VkSemaphore& Handle() const = 0;
    };
}  // namespace Engine::Vulkan::Interface