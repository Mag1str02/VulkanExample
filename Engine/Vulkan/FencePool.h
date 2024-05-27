#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Managed/Fence.h"

namespace Engine::Vulkan {

    class FencePool : public RefCounted<FencePool> {
    public:
        static Ref<FencePool> Create(Ref<Device> device);
        ~FencePool();

        Ref<IFence> CreateFence();
        Ref<Device> GetDevice() const;

    private:
        FencePool(Ref<Device> device);

    private:
        class Fence : public Managed::Fence, NonCopyMoveable {
        public:
            Fence(Ref<FencePool> pool, VkFence handle);
            ~Fence();

        private:
            Ref<FencePool> m_Pool;
        };

    private:
        std::deque<VkFence> m_Fences;
        Ref<Device>         m_Device;
    };

}  // namespace Engine::Vulkan