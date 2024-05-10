#pragma once

#include "IPassNode.h"
#include "Node.h"

namespace Engine::Vulkan::RenderGraph {

    class PassNode : public Node, public IPassNode {
    public:
        virtual ~PassNode() = default;

    protected:
        virtual Scope<Pass> CreatePass() = 0;

        virtual const std::unordered_set<Node*>& GetProducers() const final override;
        virtual const std::unordered_set<Node*>& GetConsumers() const final override;

        virtual uint32_t GetProducersCount() const final override;
        virtual uint32_t GetConsumersCount() const final override;

        virtual void AddOutput(ResourceNode& resource) final override;
        virtual void AddReadOnlyInput(ResourceNode& resource) final override;
        virtual void AddReadWriteInput(ResourceNode& resource) final override;

    private:
        friend class RenderGraph;

        std::unordered_set<Node*> m_Producers;
        std::unordered_set<Node*> m_Consumers;
    };

}  // namespace Engine::Vulkan::RenderGraph