#pragma once

#include "Common.h"
#include "Object.h"

namespace Engine::Vulkan {

    class Task : public Object {
    public:
        Task()          = default;
        virtual ~Task() = default;

        virtual void Run(Ref<Queue> queue) = 0;
        virtual void Wait()                = 0;
    };

}  // namespace Engine::Vulkan