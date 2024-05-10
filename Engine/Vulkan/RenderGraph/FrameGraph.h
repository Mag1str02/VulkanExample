#pragma once

#include "RenderGraph.h"

namespace Engine::Vulkan::RenderGraph {

    class FrameGraph final : private RenderGraph {
    public:
        FrameGraph(Ref<Surface> surface);

        virtual const std::string& GetName() const override;

        RenderGraph* GetInternalGraph() const;

    private:
        RenderGraph* m_InternalGraph = nullptr;
    };

}  // namespace Engine::Vulkan::RenderGraph