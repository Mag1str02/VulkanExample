#pragma once

#include "Engine/Vulkan/Forward.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class PassFactory {
    public:
        virtual Scope<IPass>        CreatePass()        = 0;
        virtual Scope<IPassCluster> CreatePassCluster() = 0;

        friend class ::Engine::Vulkan::RenderGraph::TaskBuilder;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface