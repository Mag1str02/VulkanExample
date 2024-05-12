#pragma once

#include "Engine/Vulkan/RenderGraph/Interface/ImageResourceNode.h"

namespace Engine::Vulkan::RenderGraph {

    class SwapChainImageNode : public ImageResourceNode {
    public:
        SwapChainImageNode(Ref<SwapChainNodesState> state);

    protected:
        virtual Ref<IImage> GetImage() override;

    private:
        Ref<SwapChainNodesState> m_State;
    };
}  // namespace Engine::Vulkan::RenderGraph