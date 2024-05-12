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
        if (!m_UnresolvedDependenciesMapping.Empty()) {
            return "Render graph has unresolved external dependencies";
        }

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

        auto unresolved_deps = m_UnresolvedDependenciesMapping.Get(name);
        for (const auto& pass : unresolved_deps) {
            DE_ASSERT(dependency_type == m_UnresolvedDependenciesTypes[pass][name], "Dependcy type mismatch");
            pass->AddExternalResource(name, resource, dependency_type);
            m_UnresolvedDependenciesTypes[pass].erase(name);
            m_UnresolvedDependenciesMapping.RemoveMapping(pass, name);
        }
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

    void RenderGraph::CreateInternalDependency(const std::string& name, ResourceNode& resource, IPassEntry& pass, DependencyType dependency_type) {
        DE_ASSERT(m_OwnedEntries.contains(&resource), "Cannot create internal dependency because resource is not owned by render graph");
        DE_ASSERT(m_OwnedEntries.contains(&pass), "Cannot create internal dependency because pass is not owned by render graph");

        pass.AddExternalResource(name, resource, dependency_type);
    }
    void RenderGraph::CreateExternalDependency(const std::string& name, IPassEntry& pass, DependencyType dependency_type) {
        DE_ASSERT(m_OwnedEntries.contains(&pass), "Cannot create external dependency because pass is not owned by render graph");
        if (auto it = m_ExternalResources.find(name); it != m_ExternalResources.end()) {
            DE_ASSERT(dependency_type == it->second.second,
                      "Dependecy type mismatch: pass_type={} external_type{}",
                      (int64_t)dependency_type,
                      (int64_t)it->second.second);
            pass.AddExternalResource(name, *it->second.first, dependency_type);
        } else {
            auto emplaced = m_UnresolvedDependenciesTypes[&pass].emplace(name, dependency_type).second;
            DE_ASSERT(emplaced, "Such external dependency already exist");
            m_UnresolvedDependenciesMapping.AddMapping(&pass, name);
        }
    }

}  // namespace Engine::Vulkan::RenderGraph