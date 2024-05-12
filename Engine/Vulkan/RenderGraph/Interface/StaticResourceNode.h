#pragma once

#include "Engine/Vulkan/RenderGraph/ResourceNode.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class StaticResourceNode : virtual public ResourceNode {
    protected:
        virtual void Instantiate() = 0;

    private:
        friend class ::Engine::Vulkan::RenderGraph::TaskBuilder;
    };
}  // namespace Engine::Vulkan::RenderGraph::Interface