#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Interface {

    class Semaphore {
    public:
        virtual ~Semaphore() = default;

        virtual VkSemaphore Handle() = 0;
    };

}  // namespace Engine::Vulkan::Interface