#pragma once

#include "DependencyType.h"

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class PassEntry {
    public:
        virtual ResourceNode* GetExternalResource(const std::string& name, DependencyType dependency_type) const = 0;

    protected:
        virtual void AddExternalResource(const std::string& name, ResourceNode& resource, DependencyType dependency_type) = 0;

    private:
        friend class ::Engine::Vulkan::RenderGraph::RenderGraph;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface