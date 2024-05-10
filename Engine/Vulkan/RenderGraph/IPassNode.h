#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::RenderGraph {

    class IPassNode {
    protected:
        virtual void AddOutput(ResourceNode& resource)         = 0;
        virtual void AddReadOnlyInput(ResourceNode& resource)  = 0;
        virtual void AddReadWriteInput(ResourceNode& resource) = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph