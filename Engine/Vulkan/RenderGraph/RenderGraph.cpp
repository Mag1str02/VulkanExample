#include "RenderGraph.h"

#include "PassNode.h"
#include "ResourceNode.h"
#include "TaskBuilder.h"

namespace Engine::Vulkan::RenderGraph {

    std::expected<Ref<Task>, std::string> RenderGraph::CreateTask(Ref<SemaphorePool> semaphore_pool) {
        PROFILER_SCOPE("Engine::Vulkan::RenderGraph::RenderGraph::CreateTask");

        if (!m_ExternalResources.empty()) {
            return std::unexpected<std::string>("Cannot create task because render graph has external resources");
        }

        auto error = Validate();
        if (error) {
            return std::unexpected<std::string>(std::format("Graph contains error: {}", *error));
        }

        TaskBuilder builder(this, semaphore_pool);
        return builder.Build();
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

    void RenderGraph::AddExternalResource(const std::string& name, ResourceNode& resource, DependencyType dependency_type) {
        bool inserted = m_ExternalResources.emplace(name, std::pair<ResourceNode*, DependencyType>(&resource, dependency_type)).second;
        DE_ASSERT(inserted == true, "Resource with name {} already exists", name);
    }
    ResourceNode* RenderGraph::GetExternalResource(const std::string& name, DependencyType dependency_type) const {
        auto it = m_ExternalResources.find(name);
        if (it == m_ExternalResources.end() || it->second.second != dependency_type) {
            return nullptr;
        }
        return it->second.first;
    }

    std::unordered_set<INode*> RenderGraph::GetNodes() const {
        std::unordered_set<INode*> nodes;
        for (const auto& [entry, _] : m_OwnedEntries) {
            if (entry->Is<INode>()) {
                nodes.emplace(entry->As<INode>());
            } else if (entry->Is<RenderGraph>()) {
                auto graph_nodes = entry->As<RenderGraph>()->GetNodes();
                nodes.insert(graph_nodes.begin(), graph_nodes.end());
            } else {
                DE_ASSERT_FAIL("Render graph owns entry that is neither node nor render graph");
            }
        }
        return nodes;
    }

    void RenderGraph::CreateInternalDependency(const std::string& name, ResourceNode& resource, IPassNode& pass, DependencyType dependency_type) {}
    void RenderGraph::CreateExternalDependency(const std::string& name, IPassNode& pass, DependencyType dependency_type) {}

}  // namespace Engine::Vulkan::RenderGraph