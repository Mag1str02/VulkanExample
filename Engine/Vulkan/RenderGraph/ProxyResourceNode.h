#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/ProxyResourceNode.h"
#include "Engine/Vulkan/RenderGraph/Interface/ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    class ProxyResourceNode : public IResourceNode, public IProxyResourceNode {
    public:
        ProxyResourceNode(ExternalDependencyManager* manager, const std::string& name);
        virtual ~ProxyResourceNode();

        virtual std::optional<std::string> Validate() const override;

        virtual IResourceNode* GetUnderlyingNode() const override;

        virtual void AddDependency(INode* node, DependencyType dependency_type) final override;
        virtual void RemoveDependency(INode* node) final override;

        virtual const std::unordered_set<INode*>& GetConsumers() const final override;
        virtual const std::unordered_set<INode*>& GetProducers() const final override;

        void SetUnderlyingNode(IResourceNode* node, DependencyType dependency_type);

        const std::string& GetName() const;
        DependencyType     GetUnderlyingDependencyType() const;

    private:
        void DeleteIfUseless();

    private:
        std::unordered_map<INode*, DependencyType> m_Dependencies;
        std::unordered_set<INode*>                 m_Consumers;
        std::unordered_set<INode*>                 m_Producers;

        uint32_t m_ReadConsumers  = 0;
        uint32_t m_WriteConsumers = 0;

        ExternalDependencyManager* m_Manager = nullptr;

        const std::string m_Name;
        IResourceNode*    m_UnderlyingResourceNode = nullptr;
        DependencyType    m_UnderlyingDependencyType;
    };

}  // namespace Engine::Vulkan::RenderGraph