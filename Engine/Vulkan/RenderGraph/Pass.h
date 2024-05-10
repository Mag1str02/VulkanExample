#pragma once

namespace Engine::Vulkan::RenderGraph {

    class Pass {
    public:
        virtual ~Pass() = default;

        virtual void Prepare() = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph