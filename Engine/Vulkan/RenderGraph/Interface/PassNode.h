#pragma once

#include "DependencyType.h"

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class PassNode {
    protected:
        virtual Scope<IPass>        CreatePass()        = 0;
        virtual Scope<IPassCluster> CreatePassCluster() = 0;

    private:
        friend class ::Engine::Vulkan::RenderGraph::TaskBuilder;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface