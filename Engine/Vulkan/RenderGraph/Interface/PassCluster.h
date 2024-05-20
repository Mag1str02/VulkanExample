#pragma once

#include "Engine/Vulkan/Renderer/SemaphorePool.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class PassCluster : NonCopyMoveable {
    public:
        virtual ~PassCluster() = default;

        virtual bool AddPass(IPass* pass)       = 0;
        virtual void Finalize()                 = 0;
        virtual void Submit(Executor* executor) = 0;

        virtual void AddWaitSemaphore(Ref<Semaphore> semaphore)   = 0;
        virtual void AddSignalSemaphore(Ref<Semaphore> semaphore) = 0;
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface