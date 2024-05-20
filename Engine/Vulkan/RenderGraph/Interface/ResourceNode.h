#pragma once

#include "Node.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class ResourceNode : public INode {
    public:
        virtual void AddDependency(INode* node, DependencyType dependency_type) = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface