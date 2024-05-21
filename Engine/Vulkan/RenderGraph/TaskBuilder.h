#pragma once

#include "RenderGraph.h"

#include "Engine/Vulkan/RenderGraph/Interface/PassCluster.h"

namespace Engine::Vulkan::RenderGraph {

    class TaskBuilder {
    public:
        TaskBuilder(RenderGraph* graph, Ref<SemaphorePool> semaphore_pool);
        Ref<Task> Build();

    private:
        class Task : public ITask {
        public:
            Task(std::vector<Scope<IPassCluster>> clusters, std::vector<Scope<IPass>> passes);

            virtual bool IsCompleted() const override;
            virtual void Run(Executor* executor) override;

        private:
            std::vector<Scope<IPassCluster>> m_PassClusters;
            std::vector<Scope<IPass>>        m_Passes;
        };

    private:
        RenderGraph*       m_Graph;
        Ref<SemaphorePool> m_SemaphorePool;
    };
}  // namespace Engine::Vulkan::RenderGraph