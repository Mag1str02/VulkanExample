#pragma once

#include "Node.h"

namespace Engine::Vulkan::RenderGraph {

    class ResourceNode : public Node {
    public:
        virtual std::optional<std::string> Validate() const override;

    protected:
        virtual void Instantiate() = 0;
        virtual void Claim()       = 0;

        virtual const std::unordered_set<Node*>& GetProducers() const final override;
        virtual const std::unordered_set<Node*>& GetConsumers() const final override;

        virtual uint32_t GetProducersCount() const final override;
        virtual uint32_t GetConsumersCount() const final override;

    private:
        void AddReadConsumer(PassNode& pass);
        void AddWriteConsumer(PassNode& pass);
        void AddProducer(PassNode& pass);

    private:
        friend class PassNode;
        friend class RenderGraph;

        std::unordered_set<Node*> m_Producers;
        std::unordered_set<Node*> m_Consumers;

        uint32_t m_WriteConsumers = 0;
        uint32_t m_ReadConsumers  = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph