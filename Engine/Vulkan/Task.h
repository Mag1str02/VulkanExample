#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class SyncRequirements {
    public:
        virtual void RecordBarriers(Managed::CommandBuffer& buffer) const = 0;
        virtual bool RequiresBarriers() const                             = 0;
        virtual bool RequiresSemaphore() const                            = 0;
    };

    class Task : public SyncRequirements {
    public:
        virtual ~Task() = default;

        virtual void Wait() const        = 0;
        virtual bool IsCompleted() const = 0;

        virtual void                    Run(VkQueue queue, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) = 0;
    };

}  // namespace Engine::Vulkan