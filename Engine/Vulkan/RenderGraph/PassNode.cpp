#include "PassNode.h"

#include "ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    const std::unordered_set<INode*>& PassNode::GetProducers() const {
        return m_Producers;
    }
    const std::unordered_set<INode*>& PassNode::GetConsumers() const {
        return m_Consumers;
    }

    uint32_t PassNode::GetProducersCount() const {
        return m_Producers.size();
    }
    uint32_t PassNode::GetConsumersCount() const {
        return m_Consumers.size();
    }

    void PassNode::AddExternalResource(const std::string& name, ResourceNode& resource, DependencyType dependency_type) {
        bool inserted = m_Resources.emplace(name, std::pair<ResourceNode*, DependencyType>(&resource, dependency_type)).second;
        DE_ASSERT(inserted == true, "Resource with name {} already exists", name);
        if (dependency_type == DependencyType::Output) {
            m_Consumers.emplace(&resource);
        } else {
            m_Producers.emplace(&resource);
        }
    }
    ResourceNode* PassNode::GetExternalResource(const std::string& name, DependencyType dependency_type) const {
        auto it = m_Resources.find(name);
        if (it == m_Resources.end() || it->second.second != dependency_type) {
            return nullptr;
        }
        return it->second.first;
    }
}  // namespace Engine::Vulkan::RenderGraph