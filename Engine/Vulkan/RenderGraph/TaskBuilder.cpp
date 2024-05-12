#include "TaskBuilder.h"

#include "Engine/Vulkan/RenderGraph/Interface/Node.h"
#include "Engine/Vulkan/RenderGraph/Interface/Pass.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassCluster.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassNode.h"
#include "Engine/Vulkan/RenderGraph/Interface/StaticResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    namespace {
        template <typename T>
        std::vector<T*> FilterNodes(const std::vector<INode*>& nodes) {
            std::vector<T*> filtered_nodes;
            for (const auto& node : nodes) {
                if (node->Is<T>()) {
                    filtered_nodes.push_back(node->As<T>());
                }
            }
            return filtered_nodes;
        }

        class SwapChainAquirePassCluster {};

    }  // namespace

    TaskBuilder::TaskBuilder(RenderGraph* graph, Ref<SemaphorePool> semaphore_pool) : m_Graph(graph), m_SemaphorePool(semaphore_pool) {};

    Ref<Task> TaskBuilder::Build() {
        PROFILER_SCOPE("Engine::Vulkan::RenderGraph::TaskBuilder::Build");

        auto nodes        = m_Graph->GetNodes();
        auto sorted_nodes = TopologicalSort(nodes);

        auto static_resource_nodes = FilterNodes<StaticResourceNode>(sorted_nodes);
        auto pass_nodes            = FilterNodes<IPassNode>(sorted_nodes);

        for (const auto& static_resource : static_resource_nodes) {
            static_resource->Instantiate();
        }
        for (const auto& pass : pass_nodes) {
            m_Passes.emplace_back(pass->CreatePass());
        }

        // TODO: execute pass->Prepare()  concurrently according to non static resource dependencies
        for (const auto& pass : m_Passes) {
            pass->Prepare();
        }

        return Ref<Task>(new Task());
    }

    std::vector<INode*> TaskBuilder::TopologicalSort(const std::unordered_set<INode*>& nodes) const {
        uint64_t                             counter = 2;
        std::unordered_map<INode*, uint64_t> counters;
        for (const auto& node : nodes) {
            counters.emplace(node, 0);
        }

        std::function<void(INode*)> dfs;
        dfs = [&counter, &counters, &dfs](INode* current_node) {
            counters[current_node] = 1;
            for (const auto& consumer : current_node->GetConsumers()) {
                DE_ASSERT(counters.contains(consumer), "Found unknow node");  // TODO: move to validation
                if (counters[consumer] == 0) {
                    dfs(consumer);
                }
            }
            counters[current_node] = counter++;
        };

        for (const auto& node : nodes) {
            if (counters[node] == 0) {
                dfs(node);
            }
        }

        std::vector<INode*> sorted_nodes(nodes.begin(), nodes.end());
        std::sort(sorted_nodes.begin(), sorted_nodes.end(), [&counters](INode* a, INode* b) { return counters[a] < counters[b]; });
        return sorted_nodes;
    }

    bool TaskBuilder::Task::IsCompleted() const {
        return true;
    }
    void TaskBuilder::Task::Run(Executor* executor) {}

}  // namespace Engine::Vulkan::RenderGraph