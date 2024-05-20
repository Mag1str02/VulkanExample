#pragma once

#include "DependencyType.h"
#include "Entry.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class PassNode : public virtual IEntry {
    public:
        virtual IResourceNode* GetExternalResource(const std::string& name, DependencyType dependency_type) const                    = 0;
        virtual void           AddExternalResource(IResourceNode* resource, const std::string& name, DependencyType dependency_type) = 0;
        virtual void           RemoveExternalResource(IResourceNode* resource)                                                       = 0;

        friend class ::Engine::Vulkan::RenderGraph::RenderGraph;
        friend class ::Engine::Vulkan::RenderGraph::ResourceNode;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface