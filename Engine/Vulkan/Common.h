#pragma once

#include "Forward.h"

#include "Engine/Utils/Base.h"

#include <volk.h>

#include <tracy/TracyVulkan.hpp>

#define VK_CHECK(...)                                                                                      \
    {                                                                                                      \
        auto _____res = __VA_ARGS__;                                                                       \
        DE_ASSERT(_____res == VK_SUCCESS, std::format("{} returned {}", #__VA_ARGS__, (int64_t)_____res)); \
    }

namespace Engine::Vulkan {

    enum class ShaderStage {
        None = 0,
        Vertex,
        Fragment,
    };

}  // namespace Engine::Vulkan