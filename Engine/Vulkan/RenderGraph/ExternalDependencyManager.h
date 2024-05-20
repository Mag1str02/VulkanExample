#pragma once

#include "ProxyResourceNode.h"

#include "Engine/Vulkan/RenderGraph/Interface/ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {
    class ExternalDependencyManager {
    public:
        IResourceNode*                    GetExternalResource(const std::string& name, DependencyType dependency_type) const;
        const std::unordered_set<INode*>& GetNodes() const;

        void AddExternalResourceRequest(IPassNode* pass, const std::string& name, DependencyType dependency_type);
        void RemoveExternalResourceRequest(IPassNode* pass, const std::string& name);

        void AddExternalResource(IResourceNode* resource, const std::string& name, DependencyType dependency_type);
        void RemoveExternalResource(IResourceNode* resource);

        void DeleteProxyNode(ProxyResourceNode* proxy_node);

    private:
        ProxyResourceNode* CreateIfNotExists(const std::string& name);

    private:
        std::unordered_map<std::string, Scope<ProxyResourceNode>> m_ProxyNodeByName;
        std::unordered_map<IResourceNode*, std::string>           m_ExternalResources;

        std::unordered_set<INode*> m_ProxyNodes;
    };
}  // namespace Engine::Vulkan::RenderGraph