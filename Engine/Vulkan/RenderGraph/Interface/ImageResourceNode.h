#pragma once

#include "Engine/Vulkan/RenderGraph/ResourceNode.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class ImageResourceNode : virtual public ResourceNode {
    protected:
        virtual Ref<IImage> GetImage() = 0;
    };
}  // namespace Engine::Vulkan::RenderGraph::Interface