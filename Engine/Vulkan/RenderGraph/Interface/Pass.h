#pragma once

#include "DynamicType.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class Pass : public DynamicType, NonCopyMoveable {
    public:
        virtual ~Pass() = default;

        virtual void Prepare() = 0;

    private:
        friend class ::Engine::Vulkan::RenderGraph::TaskBuilder;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface