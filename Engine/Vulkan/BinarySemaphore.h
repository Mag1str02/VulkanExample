#pragma once

#include "Engine/Vulkan/Interface/BinarySemaphore.h"

namespace Engine::Vulkan {

    class BinarySemaphore : public IBinarySemaphore, NonCopyMoveable {
    public:
        BinarySemaphore(Ref<Device> device);
        ~BinarySemaphore();

        virtual const VkSemaphore& Handle() const override;

    private:
        VkSemaphore m_Handle = VK_NULL_HANDLE;
        Ref<Device> m_Device;
    };

}  // namespace Engine::Vulkan