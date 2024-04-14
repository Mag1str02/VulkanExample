#pragma once

#include "Common.h"

namespace Vulkan {

    class Object : RefCounted<Object> {
    public:
        Object()          = default;
        virtual ~Object() = default;

        NO_MOVE_CONSTRUCTORS(Object);
        NO_COPY_CONSTRUCTORS(Object);
    };

}  // namespace Vulkan