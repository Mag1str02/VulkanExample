#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Interface {

    class Fence {
    public:
        virtual ~Fence() = default;

        virtual const VkFence& Handle() const     = 0;
        virtual bool           IsSignaled() const = 0;

        virtual void Wait() const = 0;
        virtual void Reset()      = 0;
    };
}  // namespace Engine::Vulkan::Interface