#include "ExternalDependencyManager.h"

#include "Engine/Vulkan/RenderGraph/Interface/PassNode.h"

namespace Engine::Vulkan::RenderGraph {

    IResourceNode* ExternalDependencyManager::GetExternalResource(const std::string& name, DependencyType dependency_type) const {
        auto proxy_it = m_ProxyNodeByName.find(name);

        if (proxy_it == m_ProxyNodeByName.end()) {
            return nullptr;
        }
        ProxyResourceNode* proxy_node          = proxy_it->second.get();
        IResourceNode*     underlying_resource = proxy_node->GetUnderlyingNode();
        if (underlying_resource == nullptr) {
            return nullptr;
        }
        if (dependency_type != proxy_node->GetUnderlyingDependencyType()) {
            return nullptr;
        }
        return underlying_resource;
    }
    const std::unordered_set<INode*>& ExternalDependencyManager::GetNodes() const {
        return m_ProxyNodes;
    }

    void ExternalDependencyManager::AddExternalResourceRequest(IPassNode* pass, const std::string& name, DependencyType dependency_type) {
        ProxyResourceNode* proxy_node = CreateIfNotExists(name);
        pass->AddExternalResource(proxy_node, name, dependency_type);
    }
    void ExternalDependencyManager::RemoveExternalResourceRequest(IPassNode* pass, const std::string& name) {
        auto res_it = m_ProxyNodeByName.find(name);
        if (res_it == m_ProxyNodeByName.end()) {
            return;
        }
        ProxyResourceNode* node = res_it->second.get();
        pass->RemoveExternalResource(node);
    }

    void ExternalDependencyManager::AddExternalResource(IResourceNode* resource, const std::string& name, DependencyType dependency_type) {
        auto inserted = m_ExternalResources.emplace(resource, name).second;
        DE_ASSERT(inserted, "External resource with name {} already exists", name);

        ProxyResourceNode* proxy_node        = CreateIfNotExists(name);
        DependencyType proxy_dependency_type = (dependency_type == DependencyType::Output ? DependencyType::ReadWriteInput : DependencyType::Output);

        resource->AddDependency(proxy_node, dependency_type);
        proxy_node->AddDependency(resource, proxy_dependency_type);
        proxy_node->SetUnderlyingNode(resource, dependency_type);
    }
    void ExternalDependencyManager::RemoveExternalResource(IResourceNode* resource) {
        auto res_it = m_ExternalResources.find(resource);
        if (res_it == m_ExternalResources.end()) {
            return;
        }
        auto  proxy_it   = m_ProxyNodeByName.find(res_it->second);
        auto* proxy_node = proxy_it->second.get();
        resource->RemoveDependency(proxy_node);
        proxy_node->RemoveDependency(resource);
    }
    ProxyResourceNode* ExternalDependencyManager::CreateIfNotExists(const std::string& name) {
        auto res_it = m_ProxyNodeByName.find(name);
        if (res_it == m_ProxyNodeByName.end()) {
            res_it = m_ProxyNodeByName.emplace(name, CreateScope<ProxyResourceNode>(this, name)).first;
            m_ProxyNodes.emplace(res_it->second.get());
        }
        return res_it->second.get();
    }

    void ExternalDependencyManager::DeleteProxyNode(ProxyResourceNode* proxy_node) {
        m_ProxyNodes.erase(proxy_node);
        m_ProxyNodeByName.erase(proxy_node->GetName());
    }
}  // namespace Engine::Vulkan::RenderGraph