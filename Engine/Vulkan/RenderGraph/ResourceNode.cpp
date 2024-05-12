#include "ResourceNode.h"

#include "PassNode.h"

namespace Engine::Vulkan::RenderGraph {

    std::optional<std::string> ResourceNode::Validate() const {
        if (GetProducersCount() > 1) {
            return {std::format("ResourceNode cannot have more than 1 producer")};
        }
        if (m_WriteConsumers != 0 && m_ReadConsumers != 0) {
            return {std::format("ResourceNode cannot have read and write consumers simultaneously")};
        }
        return std::nullopt;
    }

    const std::unordered_set<INode*>& ResourceNode::GetProducers() const {
        return m_Producers;
    }
    const std::unordered_set<INode*>& ResourceNode::GetConsumers() const {
        return m_Consumers;
    }

    uint32_t ResourceNode::GetProducersCount() const {
        return m_Producers.size();
    }
    uint32_t ResourceNode::GetConsumersCount() const {
        return m_Consumers.size();
    }

    void ResourceNode::AddDependency(PassNode& pass, DependencyType dependency_type) {
        Node* node = (Node*)&pass;
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

}  // namespace Engine::Vulkan::RenderGraph