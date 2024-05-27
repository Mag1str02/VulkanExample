#include "TaskBuilder.h"

#include "Engine/Vulkan/BinarySemaphorePool.h"
#include "Engine/Vulkan/Fence.h"
#include "Engine/Vulkan/RenderGraph/Interface/Node.h"
#include "Engine/Vulkan/RenderGraph/Interface/Pass.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassCluster.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassFactory.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassNode.h"
#include "Engine/Vulkan/RenderGraph/Interface/StaticResourceNode.h"
#include "Engine/Vulkan/RenderGraph/PassNode.h"
#include "Engine/Vulkan/RenderGraph/RenderGraph.h"

namespace Engine::Vulkan::RenderGraph {

    namespace {

        template <typename T>
        using Graph = std::unordered_map<T*, std::unordered_set<T*>>;

        template <typename T>
        Graph<T> BuildReverseGraph(const Graph<T>& graph) {
            Graph<T> result;
            for (const auto& [node, _] : graph) {
                result[node] = {};
            }
            for (const auto& [node, consumers] : graph) {
                for (const auto& consumer : consumers) {
                    result[consumer].emplace(node);
                }
            }
            return result;
        };
        template <typename TGraph, typename TNode>
        void RemoveNodes(Graph<TGraph>& graph) {
            auto                reversed_graph = BuildReverseGraph(graph);
            std::vector<INode*> nodes;
            for (const auto& [node, _] : graph) {
                if (node->template Is<TNode>()) {
                    nodes.emplace_back(node);
                }
            }
            for (const auto& node : nodes) {
                for (const auto& consumer : graph.at(node)) {
                    graph[consumer].erase(node);
                }
                for (const auto& producer : reversed_graph.at(node)) {
                    graph[producer].erase(node);
                    for (const auto& consumer : graph.at(node)) {
                        graph[producer].emplace(consumer);
                    }
                }
                graph.erase(node);
            }
        }
        template <typename T>
        std::vector<T*> TopologicalSort(const Graph<T>& graph) {
            uint64_t                         counter = 2;
            std::unordered_map<T*, uint64_t> counters;

            std::vector<T*> nodes;
            for (const auto& [node, _] : graph) {
                counters.emplace(node, 0);
                nodes.emplace_back(node);
            }

            std::function<void(T*)> dfs;
            dfs = [&counter, &counters, &dfs, &graph](T* current_node) {
                counters[current_node] = 1;
                for (const auto& consumer : graph.at(current_node)) {
                    DE_ASSERT(counters.contains(consumer), "Found unknow node");  // TODO: move to validation
                    if (counters[consumer] == 0) {
                        dfs(consumer);
                    }
                }
                counters[current_node] = counter++;
            };

            for (const auto& [node, _] : graph) {
                if (counters[node] == 0) {
                    dfs(node);
                }
            }

            std::sort(nodes.begin(), nodes.end(), [&counters](T* a, T* b) { return counters[a] > counters[b]; });

            return nodes;
        };

    }  // namespace

    TaskBuilder::TaskBuilder(RenderGraph* graph, Ref<BinarySemaphorePool> semaphore_pool) : m_Graph(graph), m_SemaphorePool(semaphore_pool) {};

    TaskBuilder::TaskObjects TaskBuilder::Build() {
        PROFILER_SCOPE("Engine::Vulkan::RenderGraph::TaskBuilder::Build");

        std::vector<Scope<IPassCluster>> pass_clusters;
        std::vector<Scope<IPass>>        passes;

        auto& m_SemaphorePool = this->m_SemaphorePool;

        //* function declaration
        auto BuildFullGraph = [](const std::unordered_set<INode*>& nodes) -> Graph<INode> {
            Graph<INode> graph;
            for (const auto& node : nodes) {
                graph[node] = {};
            }
            for (const auto& node : nodes) {
                for (const auto& consumer : node->GetConsumers()) {
                    DE_CHECK(graph.contains(consumer));
                    graph[node].emplace(consumer);
                }
                for (const auto& producer : node->GetProducers()) {
                    DE_CHECK(graph.contains(producer));
                    graph[producer].emplace(node);
                }
            }
            return graph;
        };

        auto BuildClusters = [&pass_clusters](const Graph<INode>&                       graph,
                                              const std::unordered_map<INode*, IPass*>& passes) -> std::unordered_map<INode*, IPassCluster*> {
            std::unordered_map<INode*, IPassCluster*> pass_node_to_cluster;
            std::unordered_set<INode*>                front_line;

            auto reversed_graph = BuildReverseGraph(graph);

            for (const auto& [node, consumers] : reversed_graph) {
                if (consumers.empty()) {
                    front_line.emplace(node->As<PassNode>());
                }
            }

            while (!front_line.empty()) {
                pass_clusters.emplace_back((*front_line.begin())->As<IPassFactory>()->CreatePassCluster());
                IPassCluster*      cluster = pass_clusters.back().get();
                std::queue<INode*> candidates(front_line.begin(), front_line.end());
                bool               cluster_non_empty = false;
                while (!candidates.empty()) {
                    INode* candidat = candidates.front();
                    candidates.pop();
                    if (cluster->AddPass(passes.at(candidat))) {
                        pass_node_to_cluster[candidat] = cluster;
                        cluster_non_empty              = true;
                        front_line.erase(candidat);
                        for (const auto& consumer : graph.at(candidat)) {
                            auto& consumer_producers = reversed_graph.find(consumer)->second;
                            consumer_producers.erase(candidat);
                            if (consumer_producers.empty()) {
                                front_line.emplace(consumer);
                                candidates.emplace(consumer);
                            }
                        }
                    }
                }
                DE_CHECK(cluster_non_empty);
            }
            DE_ASSERT(pass_node_to_cluster.size() == passes.size(), "{} {}", pass_node_to_cluster.size(), passes.size());
            return pass_node_to_cluster;
        };
        auto InstantiateResources = [](const Graph<INode>& graph) -> void {
            for (const auto& [node, _] : graph) {
                if (node->Is<IStaticResourceNode>()) {
                    node->As<IStaticResourceNode>()->Instantiate();
                }
            }
        };
        auto InstantiatePasses = [&passes](const Graph<INode>& graph) -> std::unordered_map<INode*, IPass*> {
            std::unordered_map<INode*, IPass*> result;
            for (const auto& [node, consumer] : graph) {
                if (node->Is<PassNode>()) {
                    passes.emplace_back(node->As<PassNode>()->CreatePass());
                    result[node] = passes.back().get();
                }
            }
            return result;
        };
        auto BuildClusterGraph = [](const Graph<INode>& graph, std::unordered_map<INode*, IPassCluster*>& node_to_cluster) -> Graph<IPassCluster> {
            Graph<IPassCluster> result;
            for (const auto& [node, _] : graph) {
                result[node_to_cluster.at(node)] = {};
            }
            for (const auto& [node, consumers] : graph) {
                for (const auto& consumer : consumers) {
                    result.at(node_to_cluster.at(node)).emplace(node_to_cluster.at(consumer));
                }
            }
            return result;
        };
        auto BuildClusterDependencies = [&m_SemaphorePool](const std::vector<IPassCluster*>& clusters) -> void {
            for (size_t i = 0; i < clusters.size(); ++i) {
                if (i + 1 < clusters.size()) {
                    auto semaphore = m_SemaphorePool->CreateSemaphore();
                    clusters[i]->AddSignalSemaphore(semaphore);
                    clusters[i + 1]->AddWaitSemaphore(std::move(semaphore));
                }
            }
        };
        auto PreparePasses = [](const std::vector<INode*>& nodes, const std::unordered_map<INode*, IPass*>& node_to_pass) -> void {
            for (const auto& node : nodes) {
                node_to_pass.at(node)->Prepare();
            }
        };

        //* 0
        auto all_nodes = m_Graph->GetNodes();

        //* 1
        Graph<INode> graph = BuildFullGraph(all_nodes);
        RemoveNodes<INode, IProxyResourceNode>(graph);

        //*2
        InstantiateResources(graph);
        auto node_to_pass = InstantiatePasses(graph);

        //*3
        RemoveNodes<INode, IResourceNode>(graph);
        auto node_to_cluster = BuildClusters(graph, node_to_pass);
        auto cluster_graph   = BuildClusterGraph(graph, node_to_cluster);

        //*4
        auto sorted_clusters = TopologicalSort(cluster_graph);
        BuildClusterDependencies(sorted_clusters);

        //*5
        auto sorted_nodes = TopologicalSort(graph);
        PreparePasses(sorted_nodes, node_to_pass);

        TaskObjects result;
        result.pass_clusters = std::move(pass_clusters);
        result.passes        = std::move(passes);
        return result;
    }

}  // namespace Engine::Vulkan::RenderGraph