#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Semaphore : NonCopyMoveable {
    public:
        ~Semaphore();

        VkSemaphore Handle();

    private:
        Semaphore(Ref<SemaphorePool> pool, VkSemaphore handle);

    private:
        friend class SemaphorePool;

        VkSemaphore        m_Handle = VK_NULL_HANDLE;
        Ref<SemaphorePool> m_Pool;
    };

    // TODO: pool shrinking
    class SemaphorePool : public RefCounted<SemaphorePool> {
    public:
        static Ref<SemaphorePool> Create(Ref<Device> device);
        ~SemaphorePool();

        Ref<Semaphore> CreateSemaphore();

    private:
        SemaphorePool(Ref<Device> device);

    private:
        friend class Semaphore;

        std::deque<VkSemaphore> m_Semaphores;
        Ref<Device>             m_Device;
    };

}  // namespace Engine::Vulkan