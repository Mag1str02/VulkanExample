#include "RenderGraph.h"

#include "PassNode.h"
#include "ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    std::expected<Ref<Task>, std::string> RenderGraph::CreateTask() {
        if (!m_ExternalInputROResource.empty()) {
            return std::unexpected<std::string>("Cannot create task because render graph has external ro input resources");
        }
        if (!m_ExternalInputRWResource.empty()) {
            return std::unexpected<std::string>("Cannot create task because render graph has external rw input resources");
        }
        if (!m_ExternalOutputResource.empty()) {
            return std::unexpected<std::string>("Cannot create task because render graph has external output resources");
        }
        auto error = Validate();

        if (error) {
            return std::unexpected<std::string>(std::format("Graph contains error: {}", *error));
        }

        return Ref<Task>(new Task(this));
    }

    std::optional<std::string> RenderGraph::Validate() const {
        for (const auto& [entry, _] : m_OwnedEntries) {
            auto error = entry->Validate();
            if (error) {
                return error;
            }
        }
        // TODO: check external resource match
        // TODO: check for graph integrity (no cycles, ...)
        return std::nullopt;
    }

    void RenderGraph::AddOutput(ResourceNode& resource) {
        bool emplaced = m_ExternalOutputResource.emplace(resource.GetName(), &resource).second;
        DE_ASSERT(emplaced == true, "Resource named {} already exists", resource.GetName());
    }
    void RenderGraph::AddReadOnlyInput(ResourceNode& resource) {
        bool emplaced = m_ExternalInputROResource.emplace(resource.GetName(), &resource).second;
        DE_ASSERT(emplaced == true, "Resource named {} already exists", resource.GetName());
    }
    void RenderGraph::AddReadWriteInput(ResourceNode& resource) {
        bool emplaced = m_ExternalInputRWResource.emplace(resource.GetName(), &resource).second;
        DE_ASSERT(emplaced == true, "Resource named {} already exists", resource.GetName());
    }

    std::unordered_set<Node*> RenderGraph::GetNodes() const {
        std::unordered_set<Node*> nodes;
        for (const auto& [entry, _] : m_OwnedEntries) {
            if (entry->Is<Node>()) {
                nodes.emplace(entry->As<Node>());
            } else if (entry->Is<RenderGraph>()) {
                auto graph_nodes = entry->As<RenderGraph>()->GetNodes();
                nodes.insert(graph_nodes.begin(), graph_nodes.end());
            } else {
                DE_ASSERT_FAIL("Render graph owns entry that is neither node nor render graph");
            }
        }
        return nodes;
    }

    RenderGraph::Task::Task(RenderGraph* graph) : m_Graph(graph) {
        auto nodes = m_Graph->GetNodes();

        auto sorted_nodes = TopologicalSort(nodes);

        auto resource_nodes = FilterNodes<ResourceNode>(sorted_nodes);
        auto pass_nodes     = FilterNodes<PassNode>(sorted_nodes);

        InstantiateResource(resource_nodes);
        CreatePasses(pass_nodes);
        ClaimResource(resource_nodes);
    }

    void RenderGraph::Task::InstantiateResource(const std::vector<ResourceNode*>& nodes) const {
        for (const auto& resource : nodes) {
            resource->Instantiate();
        }
    }

    void RenderGraph::Task::ClaimResource(const std::vector<ResourceNode*>& nodes) const {
        for (const auto& resource : nodes) {
            resource->Claim();
        }
    }

    void RenderGraph::Task::CreatePasses(const std::vector<PassNode*>& passes) {
        for (const auto& pass : passes) {
            m_Passes.emplace_back(pass->CreatePass());
        }
    }

    std::vector<Node*> RenderGraph::Task::TopologicalSort(const std::unordered_set<Node*>& nodes) const {
        uint64_t                            counter = 2;
        std::unordered_map<Node*, uint64_t> counters;
        for (const auto& node : nodes) {
            counters.emplace(node, 0);
        }

        std::function<void(Node*)> dfs;
        dfs = [&counter, &counters, &dfs](Node* current_node) {
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

        std::vector<Node*> sorted_nodes(nodes.begin(), nodes.end());
        std::sort(sorted_nodes.begin(), sorted_nodes.end(), [&counters](Node* a, Node* b) { return counters[a] < counters[b]; });
        return sorted_nodes;
    }

}  // namespace Engine::Vulkan::RenderGraph