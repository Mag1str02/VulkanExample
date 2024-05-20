#include "FrameGraph.h"

#include "SwapChainAquirePassNode.h"
#include "SwapChainImageNode.h"
#include "SwapChainPresentPassNode.h"

#include "Engine/Vulkan/RenderGraph/ResourceNode.h"
#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan::RenderGraph {

    FrameGraph::FrameGraph(Ref<Surface> surface) {
        auto shared_state = CreateRef<SwapChainNodesState>(surface);

        // auto* auquire_pass_node  = CreateEnrty<SwapChainAquirePassNode>(shared_state);
        // auto* present_pass_node  = CreateEnrty<SwapChainPresentPassNode>(shared_state);
        // auto* auquire_image_node = CreateEnrty<SwapChainImageNode>(shared_state);
        // auto* present_image_node = CreateEnrty<SwapChainImageNode>(shared_state);
        // m_InternalGraph          = CreateEnrty<RenderGraph>();

        // CreateDependency(auquire_image_node, auquire_pass_node, kAquireImageName, DependencyType::Output);
        // CreateDependency(auquire_image_node, m_InternalGraph, kAquireImageName, DependencyType::ReadWriteInput);
        // CreateDependency(present_image_node, m_InternalGraph, kPresentImageName, DependencyType::Output);
        // CreateDependency(present_image_node, present_pass_node, kPresentImageName, DependencyType::ReadOnlyInput);

        auto* auquire_pass_node = CreateEnrty<SwapChainAquirePassNode>(shared_state);
        auto* present_pass_node = CreateEnrty<SwapChainPresentPassNode>(shared_state);
        auto* image_node        = CreateEnrty<SwapChainImageNode>(shared_state);

        CreateDependency(image_node, auquire_pass_node, kAquireImageName, DependencyType::Output);
        CreateDependency(image_node, present_pass_node, kPresentImageName, DependencyType::ReadOnlyInput);
    }

    RenderGraph* FrameGraph::GetInternalGraph() const {
        return m_InternalGraph;
    }

}  // namespace Engine::Vulkan::RenderGraph