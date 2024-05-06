#pragma once

#include "Engine/Vulkan/Interface/Semaphore.h"

namespace Engine::Vulkan::Managed {
    class Semaphore : public Interface::Semaphore {
    public:
        virtual ~Semaphore() = default;

        virtual VkSemaphore Handle() override;

    protected:
        void Init(Ref<Device> device, VkSemaphore Semaphore);

    protected:
        VkSemaphore m_Handle;
        Ref<Device> m_Device;
    };
};  // namespace Engine::Vulkan::Managed