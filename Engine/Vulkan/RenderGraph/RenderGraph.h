#pragma once

#include "ExternalDependencyManager.h"
#include "ResourceNode.h"

#include "Engine/Vulkan/RenderGraph/Interface/Entry.h"
#include "Engine/Vulkan/RenderGraph/Interface/Node.h"
#include "Engine/Vulkan/RenderGraph/Interface/Pass.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassNode.h"
#include "Engine/Vulkan/RenderGraph/Interface/ProxyResourceNode.h"

#include "Engine/Vulkan/Renderer/Task.h"

namespace Engine::Vulkan::RenderGraph {

    template <typename T>
    concept CRenderGraphEntry = std::derived_from<T, PassNode> || std::derived_from<T, ResourceNode> || std::derived_from<T, RenderGraph>;

    class RenderGraph : public IPassNode {
    public:
        std::expected<Ref<Task>, std::string> CreateTask(Ref<SemaphorePool> semaphore_pool);

        virtual std::optional<std::string> Validate() const override;

        template <CRenderGraphEntry T, typename... Args>
        T* CreateEnrty(Args&&... args) {
            auto scope = CreateScope<T>(std::forward<Args>(args)...);
            T*   ptr   = scope.get();
            if constexpr (std::is_base_of_v<RenderGraph, T>) {
                ptr->m_Parent = this;
            }
            m_OwnedEntries.emplace(ptr, std::move(scope));
            return ptr;
        }
        template <CRenderGraphEntry T>
        void RemoveEntry(T* entry) {
            auto entry_it = m_OwnedEntries.find(entry);
            if (entry_it == m_OwnedEntries.end()) {
                return;
            }

            m_OwnedEntries.erase(entry_it);
        }

        void CreateDependency(IResourceNode* resource, IPassNode* pass, const std::string& name, DependencyType dependency_type);
        void RemoveDependency(IResourceNode* resource, IPassNode* pass);

        void CreateDependency(IPassNode* pass, const std::string& name, DependencyType dependency_type);
        void RemoveDependency(IPassNode* pass, const std::string& name);

    protected:
        virtual IResourceNode* GetExternalResource(const std::string& name, DependencyType dependency_type) const final override;

    private:
        virtual void AddExternalResource(IResourceNode* resource, const std::string& name, DependencyType dependency_type) final override;
        virtual void RemoveExternalResource(IResourceNode* resource) final override;

        std::unordered_set<INode*> GetNodes() const;

    private:
        friend class TaskBuilder;

        ExternalDependencyManager m_ExternalDependencyManager;

        std::unordered_map<Entry*, Scope<Entry>> m_OwnedEntries;
        RenderGraph*                             m_Parent = nullptr;
    };

}  // namespace Engine::Vulkan::RenderGraph