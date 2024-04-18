#pragma once

#include "Forward.h"

#include "Engine/Utils/Base.h"

#include <volk.h>

#define VK_CHECK(...)                               \
    {                                               \
        auto res = __VA_ARGS__;                     \
        DE_ASSERT(res == VK_SUCCESS, #__VA_ARGS__); \
    }

namespace Engine::Vulkan {

    enum class ShaderStage {
        None = 0,
        Vertex,
        Fragment,
    };

}  // namespace Engine::Vulkan