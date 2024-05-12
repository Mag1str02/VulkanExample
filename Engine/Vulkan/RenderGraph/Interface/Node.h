#pragma once

#include "DynamicType.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class Node : virtual public DynamicType {
    public:
        virtual ~Node() = default;

    protected:
        virtual const std::unordered_set<Node*>& GetProducers() const = 0;
        virtual const std::unordered_set<Node*>& GetConsumers() const = 0;

        virtual uint32_t GetProducersCount() const = 0;
        virtual uint32_t GetConsumersCount() const = 0;

    private:
        friend class ::Engine::Vulkan::RenderGraph::TaskBuilder;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface