#pragma once

#include "DependencyType.h"
#include "Entry.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class Node : public virtual IEntry {
    public:
        virtual void RemoveDependency(INode* dependency) = 0;

        virtual const std::unordered_set<Node*>& GetConsumers() const = 0;
        virtual const std::unordered_set<Node*>& GetProducers() const = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface