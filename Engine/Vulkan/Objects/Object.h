#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Object : public RefCounted<Object> {
    public:
        Object()          = default;
        virtual ~Object() = default;

        NO_MOVE_CONSTRUCTORS(Object);
        NO_COPY_CONSTRUCTORS(Object);
    };

}  // namespace Engine::Vulkan