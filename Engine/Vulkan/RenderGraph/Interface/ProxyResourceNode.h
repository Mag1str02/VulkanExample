#pragma once

#include "Engine/Vulkan/Forward.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class ProxyResourceNode {
    public:
        virtual IResourceNode* GetUnderlyingNode() const = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface