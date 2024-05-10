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
            m_OwnerEntries.emplace(ptr, std::move(scope));
            return ptr;
        }

    protected:
        virtual void AddOutput(ResourceNode& resource) final override;
        virtual void AddReadOnlyInput(ResourceNode& resource) final override;
        virtual void AddReadWriteInput(ResourceNode& resource) final override;

    private:
        class Task : public Vulkan::Task {
        public:
            Task(RenderGraph* graph);

        private:
            std::vector<Scope<Pass>> m_Passes;

            RenderGraph* m_Graph = nullptr;
        };

    private:
        std::unordered_map<Entry*, Scope<Entry>> m_OwnerEntries;

        std::unordered_map<std::string, ResourceNode*> m_ExternalOutputResource;
        std::unordered_map<std::string, ResourceNode*> m_ExternalInputROResource;
        std::unordered_map<std::string, ResourceNode*> m_ExternalInputRWResource;
    };

}  // namespace Engine::Vulkan::RenderGraph