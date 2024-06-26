#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    class ResourceNode : public IResourceNode {
    public:
        virtual ~ResourceNode();

        virtual std::optional<std::string> Validate() const override;

    private:
        virtual void AddDependency(INode* node, DependencyType dependency_type) final override;
        virtual void RemoveDependency(INode* node) final override;

        virtual const std::unordered_set<INode*>& GetConsumers() const final override;
        virtual const std::unordered_set<INode*>& GetProducers() const final override;

    private:
        std::unordered_map<INode*, DependencyType> m_Dependencies;
        std::unordered_set<INode*>                 m_Consumers;
        std::unordered_set<INode*>                 m_Producers;

        uint32_t m_ReadConsumers  = 0;
        uint32_t m_WriteConsumers = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph