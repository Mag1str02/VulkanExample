#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/Node.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassFactory.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassNode.h"

namespace Engine::Vulkan::RenderGraph {

    class PassNode : public IPassFactory, public IPassNode, public INode {
    public:
        virtual ~PassNode();

    protected:
        virtual IResourceNode* GetExternalResource(const std::string& name, DependencyType dependency_type) const final override;

    private:
        virtual void AddExternalResource(IResourceNode* resource, const std::string& name, DependencyType dependency_type) final override;
        virtual void RemoveExternalResource(IResourceNode* resource) final override;
        virtual void RemoveDependency(INode* node) final override;

        virtual const std::unordered_set<Node*>& GetConsumers() const final override;
        virtual const std::unordered_set<Node*>& GetProducers() const final override;

        IResourceNode* GetUnderlyingResource(IResourceNode* resource) const;

    private:
        std::unordered_map<std::string, IResourceNode*>    m_ResourceByName;
        std::unordered_map<IResourceNode*, std::string>    m_ResourceNames;
        std::unordered_map<IResourceNode*, DependencyType> m_ResourceTypes;

        std::unordered_set<INode*> m_Consumers;
        std::unordered_set<INode*> m_Producers;
    };

}  // namespace Engine::Vulkan::RenderGraph