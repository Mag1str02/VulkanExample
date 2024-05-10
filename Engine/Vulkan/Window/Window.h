#pragma once

#include "Engine/Application/Window.h"
#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/RenderGraph/FrameGraph.h"

namespace Engine::Vulkan {

    class Window : public Engine::Window {
    public:
        Window(Renderer* renderer);
        ~Window();

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        virtual RenderGraph::RenderGraph* GetRenderGraph() const;

    private:
        ManualLifetime<RenderGraph::FrameGraph> m_FrameGraph;

        Renderer* m_Renderer = nullptr;
    };
}  // namespace Engine::Vulkan