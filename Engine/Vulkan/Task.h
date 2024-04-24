#pragma once

#include "Object.h"

#include "Common.h"

namespace Engine::Vulkan {

    class Task : public Object {
    public:
        Task()          = default;
        virtual ~Task() = default;

        virtual void Run(VkQueue queue) = 0;
        virtual bool IsCompleted()         = 0;
    };

}  // namespace Engine::Vulkan