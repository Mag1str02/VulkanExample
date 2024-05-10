#include "FrameGraph.h"

#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan::RenderGraph {

    namespace {
        // class SwapChainImageAquire : public ResourceNode {
        // public:
        // private:
        // };
    }  // namespace

    FrameGraph::FrameGraph(Ref<Surface> surface) : m_Surface(surface) {
        m_SwapChain = SwapChain::Create(m_Surface);
    }

}  // namespace Engine::Vulkan::RenderGraph