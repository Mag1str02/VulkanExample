#pragma once

#include "Pass.h"
#include "PassNode.h"

#include "Engine/Vulkan/Renderer/Task.h"

namespace Engine::Vulkan::RenderGraph {

    class RenderGraph : public IPassNode, public Entry {
    public:
        std::expected<Ref<Task>, std::string> CreateTask();

        virtual std::optional<std::string> Validate() const override;

        template <typename T, typename... Args>
        T* CreateEnrty(Args&&... args) {
            auto scope = CreateScope<T>(std::forward<Args>(args)...);
            T*   ptr   = scope.get();
            m_OwnedEntries.emplace(ptr, std::move(scope));
            return ptr;
        }

    protected:
        virtual void AddOutput(ResourceNode& resource) final override;
        virtual void AddReadOnlyInput(ResourceNode& resource) final override;
        virtual void AddReadWriteInput(ResourceNode& resource) final override;

    private:
        std::unordered_set<Node*> GetNodes() const;

    private:
        class Task : public Vulkan::Task {
        public:
            Task(RenderGraph* graph);

        private:
            std::vector<Node*> TopologicalSort(const std::unordered_set<Node*>& nodes) const;

            void InstantiateResource(const std::vector<ResourceNode*>& resources) const;
            void ClaimResource(const std::vector<ResourceNode*>& resources) const;

            void CreatePasses(const std::vector<PassNode*>& passes);

            template <typename T>
            std::vector<T*> FilterNodes(const std::vector<Node*>& nodes) const {
                std::vector<T*> filtered_nodes;
                for (const auto& node : nodes) {
                    if (node->Is<T>()) {
                        filtered_nodes.push_back(node->As<T>());
                    }
                }
                return filtered_nodes;
            }

        private:
            std::vector<Scope<Pass>> m_Passes;
            RenderGraph*             m_Graph = nullptr;
        };

    private:
        std::unordered_map<Entry*, Scope<Entry>> m_OwnedEntries;

        std::unordered_map<std::string, ResourceNode*> m_ExternalOutputResource;
        std::unordered_map<std::string, ResourceNode*> m_ExternalInputROResource;
        std::unordered_map<std::string, ResourceNode*> m_ExternalInputRWResource;
    };

}  // namespace Engine::Vulkan::RenderGraph