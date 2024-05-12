#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/Entry.h"
#include "Engine/Vulkan/RenderGraph/Interface/Pass.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassEntry.h"

#include "Engine/Vulkan/Renderer/Task.h"

namespace Engine::Vulkan::RenderGraph {

    class RenderGraph : public virtual IEntry, public IPassEntry {
    public:
        template <typename T, typename... Args>
        T* CreateEnrty(Args&&... args) {
            auto scope = CreateScope<T>(std::forward<Args>(args)...);
            T*   ptr   = scope.get();
            m_OwnedEntries.emplace(ptr, std::move(scope));
            return ptr;
        }

        std::expected<Ref<Task>, std::string> CreateTask(Ref<SemaphorePool> semaphore_pool);

        void CreateInternalDependency(const std::string& name, ResourceNode& resource, IPassEntry& pass, DependencyType dependency_type);
        void CreateExternalDependency(const std::string& name, IPassEntry& pass, DependencyType dependency_type);

        virtual std::optional<std::string> Validate() const override;

    private:
        virtual void          AddExternalResource(const std::string& name, ResourceNode& resource, DependencyType dependency_type) final override;
        virtual ResourceNode* GetExternalResource(const std::string& name, DependencyType dependency_type) const final override;

        std::unordered_set<INode*> GetNodes() const;

    private:
        friend class TaskBuilder;

        std::unordered_map<Entry*, Scope<Entry>>                                  m_OwnedEntries;
        std::unordered_map<std::string, std::pair<ResourceNode*, DependencyType>> m_ExternalResources;

        Mapping<IPassEntry*, std::string>                                                m_UnresolvedDependenciesMapping;
        std::unordered_map<IPassEntry*, std::unordered_map<std::string, DependencyType>> m_UnresolvedDependenciesTypes;
    };

}  // namespace Engine::Vulkan::RenderGraph