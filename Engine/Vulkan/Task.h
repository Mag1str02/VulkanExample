#pragma once

#include "Common.h"
#include "Object.h"

#include "Engine/Vulkan/Synchronization/State.h"

namespace Engine::Vulkan {

    class Task : public Object {
    public:
        struct SynchronizationRequirements {
            std::unordered_map<Ref<IImage>, Synchronization::ImageRequirements> image_requirements;
        };

    public:
        virtual ~Task() = default;

        // virtual SynchronizationRequirements GetSynchronizationRequirements() const = 0;
        // virtual Ref<Semaphore>              GetSemaphore() const                   = 0;
        virtual Ref<const IFence> GetFence() const = 0;

        virtual void Run(VkQueue queue) = 0;

    protected:
        Task() = default;
    };

}  // namespace Engine::Vulkan