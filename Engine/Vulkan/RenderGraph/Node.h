#pragma once

#include "Entry.h"

namespace Engine::Vulkan::RenderGraph {

    class Node : public Entry {
    public:
        virtual ~Node() = default;

    protected:
        friend class RenderGraph;

        virtual const std::unordered_set<Node*>& GetProducers() const = 0;
        virtual const std::unordered_set<Node*>& GetConsumers() const = 0;

        virtual uint32_t GetProducersCount() const = 0;
        virtual uint32_t GetConsumersCount() const = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph