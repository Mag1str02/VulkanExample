#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Interface/BinarySemaphore.h"

namespace Engine::Vulkan {

    class BinarySemaphorePool : public RefCounted<BinarySemaphorePool> {
    public:
        static Ref<BinarySemaphorePool> Create(Ref<Device> device);
        ~BinarySemaphorePool();

        Ref<IBinarySemaphore> CreateSemaphore();
        Ref<Device>           GetDevice() const;

    private:
        BinarySemaphorePool(Ref<Device> device);

    private:
        class BinarySemaphore : public IBinarySemaphore, NonCopyMoveable {
        public:
            BinarySemaphore(Ref<BinarySemaphorePool> pool, VkSemaphore handle);
            ~BinarySemaphore();

            virtual const VkSemaphore& Handle() const override;

        private:
            VkSemaphore        m_Handle = VK_NULL_HANDLE;
            Ref<BinarySemaphorePool> m_Pool;
        };

    private:
        std::deque<VkSemaphore> m_Semaphores;
        Ref<Device>             m_Device;
    };

}  // namespace Engine::Vulkan