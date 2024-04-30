#pragma once

#include "Engine/Vulkan/Interface/Fence.h"

namespace Engine::Vulkan::Managed {

    class Fence : public Interface::Fence {
    public:
        virtual ~Fence() = default;

        virtual VkFence Handle() override;
        virtual bool    IsSignaled() const override;

        virtual void Wait() const override;
        virtual void Reset() override;

    protected:
        Fence() = default;

        void Init(Ref<Device> device, VkFence fence);

    protected:
        Ref<Device> m_Device;
        VkFence     m_Handle = VK_NULL_HANDLE;
    };

}  // namespace Engine::Vulkan::Managed