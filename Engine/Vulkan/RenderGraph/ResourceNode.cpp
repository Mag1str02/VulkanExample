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

    const std::unordered_set<Node*>& ResourceNode::GetProducers() const {
        return m_Producers;
    }
    const std::unordered_set<Node*>& ResourceNode::GetConsumers() const {
        return m_Consumers;
    }

    uint32_t ResourceNode::GetProducersCount() const {
        return m_Producers.size();
    }
    uint32_t ResourceNode::GetConsumersCount() const {
        return m_Consumers.size();
    }

    void ResourceNode::AddReadConsumer(PassNode& pass) {
        ++m_ReadConsumers;
        m_Consumers.emplace(&pass);
    }
    void ResourceNode::AddWriteConsumer(PassNode& pass) {
        ++m_WriteConsumers;
        m_Consumers.emplace(&pass);
    }

    void ResourceNode::AddProducer(PassNode& pass) {
        m_Producers.emplace(&pass);
    }

}  // namespace Engine::Vulkan::RenderGraph