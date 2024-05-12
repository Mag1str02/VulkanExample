
#include "SwapChainImageNode.h"

#include "SwapChainNodesState.h"

namespace Engine::Vulkan::RenderGraph {

    SwapChainImageNode::SwapChainImageNode(Ref<SwapChainNodesState> state) : m_State(state) {}

    Ref<IImage> SwapChainImageNode::GetImage() {
        return m_State->GetCurrentIteration()->GetCurrentImage();
    }

}  // namespace Engine::Vulkan::RenderGraph