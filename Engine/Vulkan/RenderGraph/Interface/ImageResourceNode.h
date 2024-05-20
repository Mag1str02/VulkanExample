#pragma once

#include "ResourceNode.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class ImageResourceNode {
    public:
        virtual Ref<IImage> GetImage() = 0;
    };
}  // namespace Engine::Vulkan::RenderGraph::Interface