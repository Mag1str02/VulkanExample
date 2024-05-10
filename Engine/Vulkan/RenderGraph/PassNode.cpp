#include "PassNode.h"

#include "ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    void PassNode::AddOutput(ResourceNode& resource) {
        m_Consumers.emplace(&resource);
    }
    void PassNode::AddReadOnlyInput(ResourceNode& resource) {
        m_Producers.emplace(&resource);
    }
    void PassNode::AddReadWriteInput(ResourceNode& resource) {
        m_Producers.emplace(&resource);
    }

    const std::unordered_set<Node*>& PassNode::GetProducers() const {
        return m_Producers;
    }
    const std::unordered_set<Node*>& PassNode::GetConsumers() const {
        return m_Consumers;
    }

    uint32_t PassNode::GetProducersCount() const {
        return m_Producers.size();
    }
    uint32_t PassNode::GetConsumersCount() const {
        return m_Consumers.size();
    }
}  // namespace Engine::Vulkan::RenderGraph