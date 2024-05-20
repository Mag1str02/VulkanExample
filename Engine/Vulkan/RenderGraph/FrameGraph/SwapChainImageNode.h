#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/ImageResourceNode.h"
#include "Engine/Vulkan/RenderGraph/ResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    class SwapChainImageNode : public IImageResourceNode, public ResourceNode {
    public:
        SwapChainImageNode(Ref<SwapChainNodesState> state);

    protected:
        virtual Ref<IImage> GetImage() override;

    private:
        Ref<SwapChainNodesState> m_State;
    };
}  // namespace Engine::Vulkan::RenderGraph