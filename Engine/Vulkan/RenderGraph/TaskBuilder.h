#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/PassCluster.h"

namespace Engine::Vulkan::RenderGraph {

    class TaskBuilder {
    public:
        struct TaskObjects {
            std::vector<Scope<IPass>>        passes;
            std::vector<Scope<IPassCluster>> pass_clusters;
        };

        TaskBuilder(RenderGraph* graph, Ref<BinarySemaphorePool> semaphore_pool);
        TaskObjects Build();

    private:
        RenderGraph*             m_Graph;
        Ref<BinarySemaphorePool> m_SemaphorePool;
    };
}  // namespace Engine::Vulkan::RenderGraph