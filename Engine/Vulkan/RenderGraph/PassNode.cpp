#include "PassNode.h"

#include "ResourceNode.h"

#include "Engine/Vulkan/RenderGraph/Interface/ProxyResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    PassNode::~PassNode() {
        for (const auto& [resource, _] : m_ResourceNames) {
            resource->RemoveDependency(this);
        }
    }

    IResourceNode* PassNode::GetExternalResource(const std::string& name, DependencyType dependency_type) const {
        auto res_it = m_ResourceByName.find(name);
        if (res_it == m_ResourceByName.end()) {
            return nullptr;
        }
        auto dep_it = m_ResourceTypes.find(res_it->second);
        if (dep_it->second != dependency_type) {
            return nullptr;
        }
        return GetUnderlyingResource(res_it->second);
    }

    void PassNode::AddExternalResource(IResourceNode* resource, const std::string& name, DependencyType dependency_type) {
        bool inserted = m_ResourceNames.emplace(resource, name).second;
        DE_ASSERT(inserted == true, "Pass Node already linked to this node");
        inserted = m_ResourceByName.emplace(name, resource).second;
        DE_ASSERT(inserted == true, "Pass Node already has external resource node with name {}", name);
        m_ResourceTypes.emplace(resource, dependency_type);

        if (dependency_type == DependencyType::Output) {
            m_Consumers.emplace(resource);
        } else {
            m_Producers.emplace(resource);
        }
        resource->AddDependency(this, dependency_type);
    }
    void PassNode::RemoveExternalResource(IResourceNode* resource) {
        RemoveDependency(resource);
        resource->RemoveDependency(this);
    }

    void PassNode::RemoveDependency(INode* node) {
        IResourceNode* resource = node->As<IResourceNode>();
        auto           res_it   = m_ResourceNames.find(resource);
        if (res_it != m_ResourceNames.end()) {
            auto type_it = m_ResourceTypes.find(resource);
            if (type_it->second == DependencyType::Output) {
                m_Consumers.erase(resource);
            } else {
                m_Producers.erase(resource);
            }

            m_ResourceByName.erase(res_it->second);
            m_ResourceTypes.erase(type_it);
            m_ResourceNames.erase(res_it);
        }
    }

    const std::unordered_set<INode*>& PassNode::GetConsumers() const {
        return m_Consumers;
    }
    const std::unordered_set<INode*>& PassNode::GetProducers() const {
        return m_Producers;
    }

    IResourceNode* PassNode::GetUnderlyingResource(IResourceNode* resource) const {
        if (resource->Is<IProxyResourceNode>()) {
            return resource->As<IProxyResourceNode>()->GetUnderlyingNode();
        }
        return resource;
    }

}  // namespace Engine::Vulkan::RenderGraph