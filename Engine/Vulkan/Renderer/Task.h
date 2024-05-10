#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Task {
    public:
        virtual ~Task() = default;
    };

}  // namespace Engine::Vulkan