#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Object.h"

namespace Engine::Vulkan::Interface {

    class Semaphore : public Object {
    public:
        virtual ~Semaphore() = default;

        virtual VkSemaphore Handle() = 0;
        virtual void        Reset()  = 0;
    };

}  // namespace Engine::Vulkan::Interface