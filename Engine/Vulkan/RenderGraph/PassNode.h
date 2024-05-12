#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/Entry.h"
#include "Engine/Vulkan/RenderGraph/Interface/Node.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassEntry.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassNode.h"

namespace Engine::Vulkan::RenderGraph {

    class PassNode : public IEntry, protected IPassNode, IPassEntry, INode {
    public:
        virtual ~PassNode() = default;

        virtual ResourceNode* GetExternalResource(const std::string& name, DependencyType dependency_type) const final override;

    private:
        virtual void AddExternalResource(const std::string& name, ResourceNode& resource, DependencyType dependency_type) final override;

        virtual const std::unordered_set<INode*>& GetProducers() const final override;
        virtual const std::unordered_set<INode*>& GetConsumers() const final override;

        virtual uint32_t GetProducersCount() const final override;
        virtual uint32_t GetConsumersCount() const final override;

    private:
        friend class TaskBuilder;
        friend class DynamicType;

        std::unordered_map<std::string, std::pair<ResourceNode*, DependencyType>> m_Resources;

        std::unordered_set<Node*> m_Producers;
        std::unordered_set<Node*> m_Consumers;
    };

}  // namespace Engine::Vulkan::RenderGraph