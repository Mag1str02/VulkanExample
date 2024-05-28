#pragma once

#include "Engine/Vulkan/Interface/Fence.h"

namespace Engine::Vulkan::Managed {

    class Fence : public IFence {
    public:
        Fence(VkDevice device, VkFence fence);
        virtual ~Fence() = default;

        virtual const VkFence& Handle() const override;
        virtual bool           IsSignaled() const override;

        virtual void Wait() const override;
        virtual void Reset() override;

    protected:
        Fence() = default;
        void Init(VkDevice device, VkFence fence);

    protected:
        VkDevice m_Device;
        VkFence  m_Handle = VK_NULL_HANDLE;
    };

}  // namespace Engine::Vulkan::Managed