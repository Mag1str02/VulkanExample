#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Interface {

    class Task {
    public:
        virtual ~Task() = default;

    protected:
        virtual bool IsCompleted() const     = 0;
        virtual void Run(Executor* executor) = 0;

    private:
        friend class ::Engine::Vulkan::Executor;
    };

}  // namespace Engine::Vulkan::Interface