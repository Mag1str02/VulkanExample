#pragma once

#include "Engine/Vulkan/Forward.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class StaticResourceNode {
    public:
        virtual void Instantiate() = 0;
    };
}  // namespace Engine::Vulkan::RenderGraph::Interface