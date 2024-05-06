#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Task {
    public:
        virtual ~Task() = default;

        virtual void Wait() const        = 0;
        virtual bool IsCompleted() const = 0;

        virtual void Run(VkQueue queue) = 0;
    };

}  // namespace Engine::Vulkan::Execution