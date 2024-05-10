#pragma once

#include "RenderGraph.h"

namespace Engine::Vulkan::RenderGraph {

    class FrameGraph : public RenderGraph {
    public:
        FrameGraph(Ref<Surface> surface);

    private:
        Ref<SwapChain> m_SwapChain;
        Ref<Surface>   m_Surface;
    };

}  // namespace Engine::Vulkan::RenderGraph