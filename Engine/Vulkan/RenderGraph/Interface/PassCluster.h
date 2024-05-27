#pragma once

#include "Engine/Vulkan/Forward.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class PassCluster : NonCopyMoveable {
    public:
        virtual ~PassCluster() = default;

        virtual bool AddPass(IPass* pass)       = 0;
        virtual void Submit(Executor* executor) = 0;

        virtual void AddWaitSemaphore(Ref<IBinarySemaphore> semaphore)   = 0;
        virtual void AddSignalSemaphore(Ref<IBinarySemaphore> semaphore) = 0;
        virtual bool IsCompleted() const                          = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface