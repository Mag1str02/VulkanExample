#pragma once

#include "Object.h"

namespace Engine::Vulkan {

    class IFence {
    public:
        IFence()          = default;
        virtual ~IFence() = default;

        VkFence Handle();

        bool IsSignaled() const;
        void Wait() const;
        void Reset();

    protected:
        Ref<Device> m_Device;
        VkFence     m_Fence = VK_NULL_HANDLE;
    };

    class Fence : public IFence {
    public:
        Fence(Ref<Device> device);
        ~Fence();
    };

};  // namespace Engine::Vulkan