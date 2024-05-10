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
        for (const auto& [entry, _] : m_OwnerEntries) {
            auto error = entry->Validate();
            if (error) {
                return error;
            }
        }
        return std::nullopt;
    }

    void RenderGraph::AddOutput(ResourceNode& resource) {
        bool emplaced = m_ExternalOutputResource.emplace(resource.GetName(), &resource).second;
        DE_ASSERT(emplaced == false, "Resource named {} already exists", resource.GetName());
    }
    void RenderGraph::AddReadOnlyInput(ResourceNode& resource) {
        bool emplaced = m_ExternalInputROResource.emplace(resource.GetName(), &resource).second;
        DE_ASSERT(emplaced == false, "Resource named {} already exists", resource.GetName());
    }
    void RenderGraph::AddReadWriteInput(ResourceNode& resource) {
        bool emplaced = m_ExternalInputRWResource.emplace(resource.GetName(), &resource).second;
        DE_ASSERT(emplaced == false, "Resource named {} already exists", resource.GetName());
    }

    RenderGraph::Task::Task(RenderGraph* graph) : m_Graph(graph) {}
}  // namespace Engine::Vulkan::RenderGraph