#pragma once

#include "DynamicType.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class Pass : public DynamicType, NonCopyMoveable {
    public:
        virtual ~Pass() = default;

        virtual void Prepare() = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface