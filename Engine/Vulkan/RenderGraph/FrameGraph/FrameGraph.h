#pragma once

#include "Engine/Vulkan/RenderGraph/RenderGraph.h"

namespace Engine::Vulkan::RenderGraph {

    class FrameGraph final : public RenderGraph {
    public:
        FrameGraph(Ref<Surface> surface);

        RenderGraph* GetInternalGraph() const;

    private:
        RenderGraph* m_InternalGraph = nullptr;
    };

}  // namespace Engine::Vulkan::RenderGraph