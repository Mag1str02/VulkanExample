#include "ProxyResourceNode.h"

#include "ExternalDependencyManager.h"

namespace Engine::Vulkan::RenderGraph {

    IResourceNode* ProxyResourceNode::GetUnderlyingNode() const {
        if (m_UnderlyingResourceNode == nullptr) {
            return nullptr;
        }
        if (m_UnderlyingResourceNode->Is<ProxyResourceNode>()) {
            return m_UnderlyingResourceNode->As<ProxyResourceNode>()->GetUnderlyingNode();
        }
        return m_UnderlyingResourceNode;
    }

    ProxyResourceNode::ProxyResourceNode(ExternalDependencyManager* manager, const std::string& name) : m_Manager(manager), m_Name(name) {}
    ProxyResourceNode::~ProxyResourceNode() {
        for (const auto& [node, _] : m_Dependencies) {
            node->RemoveDependency(this);
        }
    }

    void ProxyResourceNode::AddDependency(INode* node, DependencyType dependency_type) {
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
    void ProxyResourceNode::RemoveDependency(INode* node) {
        auto it = m_Dependencies.find(node);
        if (it == m_Dependencies.end()) {
            return;
        }
        if (node == m_UnderlyingResourceNode) {
            m_UnderlyingResourceNode = nullptr;
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
        DeleteIfUseless();
    }

    const std::unordered_set<INode*>& ProxyResourceNode::GetConsumers() const {
        return m_Consumers;
    }
    const std::unordered_set<INode*>& ProxyResourceNode::GetProducers() const {
        return m_Producers;
    }

    std::optional<std::string> ProxyResourceNode::Validate() const {
        if (m_UnderlyingResourceNode == nullptr) {
            return std::format("ProxyResourceNode has no underlying resource node");
        }
        if (m_Producers.size() > 1) {
            return std::format("ProxyResourceNode cannot have more than 1 producer");
        }
        if (m_WriteConsumers != 0 && m_ReadConsumers != 0) {
            return std::format("ProxyResourceNode cannot have read and write consumers simultaneously");
        }
        if (m_WriteConsumers > 1) {
            return {std::format("ResourceNode cannot have multiple write consumers simultaneously")};
        }
        return std::nullopt;
    }
    void ProxyResourceNode::DeleteIfUseless() {
        if (m_Dependencies.empty()) {
            m_Manager->DeleteProxyNode(this);
        }
    }
    const std::string& ProxyResourceNode::GetName() const {
        return m_Name;
    }
    DependencyType ProxyResourceNode::GetUnderlyingDependencyType() const {
        DE_CHECK(m_UnderlyingResourceNode != nullptr);
        return m_UnderlyingDependencyType;
    }
    void ProxyResourceNode::SetUnderlyingNode(IResourceNode* node, DependencyType dependency_type) {
        DE_CHECK(m_UnderlyingResourceNode == nullptr);
        DE_CHECK(m_Dependencies.contains(node));
        m_UnderlyingResourceNode   = node;
        m_UnderlyingDependencyType = dependency_type;
    }

}  // namespace Engine::Vulkan::RenderGraph