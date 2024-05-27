#include "RenderGraph.h"

#include "PassNode.h"
#include "TaskBuilder.h"

#include "Engine/Vulkan/BinarySemaphorePool.h"
#include "Engine/Vulkan/RenderGraph/Interface/ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    RenderGraph::RenderGraph(Ref<Device> device) : m_TaskBuilder(this, BinarySemaphorePool::Create(std::move(device))) {}

    std::expected<Ref<ITask>, std::string> RenderGraph::CreateTask() {
        PROFILER_SCOPE("Engine::Vulkan::RenderGraph::RenderGraph::CreateTask");

        auto error = CreateTaskCheck();
        if (error.has_value()) {
            return std::unexpected<std::string>(error.value());
        }

        auto objects = m_TaskBuilder.Build();
        return Ref<Task>(new Task(std::move(objects)));
    }

    std::optional<std::string> RenderGraph::CreateTaskCheck() {
        if (m_Parent != nullptr) {
            return "Cannot create task because render graph is used in other render graph";
        }

        auto error = Validate();
        if (error) {
            return std::format("Graph contains error: {}", *error);
        }
        return std::nullopt;
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

    void RenderGraph::AddExternalResource(IResourceNode* resource, const std::string& name, DependencyType dependency_type) {
        m_ExternalDependencyManager.AddExternalResource(resource, name, dependency_type);
    }
    IResourceNode* RenderGraph::GetExternalResource(const std::string& name, DependencyType dependency_type) const {
        return m_ExternalDependencyManager.GetExternalResource(name, dependency_type);
    }

    void RenderGraph::RemoveExternalResource(IResourceNode* resource) {
        m_ExternalDependencyManager.RemoveExternalResource(resource);
    }

    std::unordered_set<INode*> RenderGraph::GetNodes() const {
        std::unordered_set<INode*> res;
        for (const auto& [entry, _] : m_OwnedEntries) {
            if (entry->Is<INode>()) {
                res.emplace(entry->As<INode>());
            } else if (entry->Is<RenderGraph>()) {
                auto nodes = entry->As<RenderGraph>()->GetNodes();
                res.insert(nodes.begin(), nodes.end());
            } else {
                DE_UNREACHABLE();
            }
        }
        const auto& proxy_nodes = m_ExternalDependencyManager.GetNodes();
        res.insert(proxy_nodes.begin(), proxy_nodes.end());
        return res;
    }

    void RenderGraph::CreateDependency(IResourceNode* resource, IPassNode* pass, const std::string& name, DependencyType dependency_type) {
        DE_ASSERT(m_OwnedEntries.contains(resource), "Cannot create internal dependency because resource is not owned by render graph");
        DE_ASSERT(m_OwnedEntries.contains(pass), "Cannot create internal dependency because pass is not owned by render graph");

        pass->AddExternalResource(resource, name, dependency_type);
    }

    void RenderGraph::RemoveDependency(IResourceNode* resource, IPassNode* pass) {
        DE_ASSERT(m_OwnedEntries.contains(resource), "Cannot remove internal dependency because resource is not owned by render graph");
        DE_ASSERT(m_OwnedEntries.contains(pass), "Cannot remove internal dependency because pass is not owned by render graph");

        pass->RemoveExternalResource(resource);
    }

    void RenderGraph::CreateDependency(IPassNode* pass, const std::string& resource_name, DependencyType dependency_type) {
        DE_ASSERT(m_OwnedEntries.contains(pass), "Cannot create external dependency because pass is not owned by render graph");
        m_ExternalDependencyManager.AddExternalResourceRequest(pass, resource_name, dependency_type);
    }

    void RenderGraph::RemoveDependency(IPassNode* pass, const std::string& resource_name) {
        m_ExternalDependencyManager.RemoveExternalResourceRequest(pass, resource_name);
    }

    RenderGraph::Task::Task(TaskBuilder::TaskObjects task_objects) :
        m_Passes(std::move(task_objects.passes)), m_PassClusters(std::move(task_objects.pass_clusters)) {}

    bool RenderGraph::Task::IsCompleted() const {
        bool completed = true;
        for (const auto& cluster : m_PassClusters) {
            if (!cluster->IsCompleted()) {
                completed = false;
                break;
            }
        }
        return completed;
    }
    void RenderGraph::Task::Run(Executor* executor) {
        for (const auto& cluster : m_PassClusters) {
            cluster->Submit(executor);
        }
    }
}  // namespace Engine::Vulkan::RenderGraph