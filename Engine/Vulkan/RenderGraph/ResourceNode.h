#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/DependencyType.h"
#include "Engine/Vulkan/RenderGraph/Interface/Entry.h"
#include "Engine/Vulkan/RenderGraph/Interface/Node.h"

namespace Engine::Vulkan::RenderGraph {

    class ResourceNode : public INode, public IEntry {
    public:
        virtual std::optional<std::string> Validate() const override;

    protected:
        virtual const std::unordered_set<INode*>& GetProducers() const final override;
        virtual const std::unordered_set<INode*>& GetConsumers() const final override;

        virtual uint32_t GetProducersCount() const final override;
        virtual uint32_t GetConsumersCount() const final override;

    private:
        void AddDependency(PassNode& pass, DependencyType dependency_type);

    private:
        friend class PassNode;

        std::unordered_set<Node*> m_Producers;
        std::unordered_set<Node*> m_Consumers;

        uint32_t m_WriteConsumers = 0;
        uint32_t m_ReadConsumers  = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph