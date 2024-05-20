#include "ResourceNode.h"

#include "Engine/Vulkan/RenderGraph/Interface/PassNode.h"

namespace Engine::Vulkan::RenderGraph {

    ResourceNode::~ResourceNode() {
        for (const auto& [node, _] : m_Dependencies) {
            node->RemoveDependency(this);
        }
    }

    std::optional<std::string> ResourceNode::Validate() const {
        if (m_Producers.size() > 1) {
            return {std::format("ResourceNode cannot have more than 1 producer")};
        }
        if (m_WriteConsumers != 0 && m_ReadConsumers != 0) {
            return {std::format("ResourceNode cannot have read and write consumers simultaneously")};
        }
        if (m_WriteConsumers > 1) {
            return {std::format("ResourceNode cannot have multiple write consumers simultaneously")};
        }
        return std::nullopt;
    }

    void ResourceNode::AddDependency(INode* node, DependencyType dependency_type) {
        DE_ASSERT(!node->Is<ResourceNode>(), "Cannot add resource node as a dependency");

        auto inserted = m_Dependencies.emplace(node, dependency_type).second;
        DE_ASSERT(inserted, "Resource node already has such dependency");

        switch (dependency_type) {
            case DependencyType::Output: m_Producers.emplace(node); break;
            case DependencyType::ReadOnlyInput:
                m_Consumers.emplace(node);
                ++m_ReadConsumers;
                break;
            case DependencyType::ReadWriteInput:
                m_Consumers.emplace(node);
                ++m_WriteConsumers;
                break;
        }
    }
    void ResourceNode::RemoveDependency(INode* node) {
        auto it = m_Dependencies.find(node);
        if (it == m_Dependencies.end()) {
            return;
        }
        switch (it->second) {
            case DependencyType::Output: m_Producers.erase(node); break;
            case DependencyType::ReadOnlyInput:
                m_Consumers.erase(node);
                --m_ReadConsumers;
                break;
            case DependencyType::ReadWriteInput:
                m_Consumers.erase(node);
                --m_WriteConsumers;
                break;
        }
        m_Dependencies.erase(it);
    }

    const std::unordered_set<INode*>& ResourceNode::GetConsumers() const {
        return m_Consumers;
    }
    const std::unordered_set<INode*>& ResourceNode::GetProducers() const {
        return m_Producers;
    }
}  // namespace Engine::Vulkan::RenderGraph