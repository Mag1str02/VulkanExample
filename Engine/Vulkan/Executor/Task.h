#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Objects/Object.h"

namespace Engine::Vulkan {

    class Task : public Object {
    public:
        Task()          = default;
        virtual ~Task() = default;

        virtual void Run(Ref<Queue> queue) = 0;
        virtual void Wait()                = 0;
    };

}  // namespace Engine::Vulkan