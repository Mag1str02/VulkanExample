#pragma once

#include "Engine/Vulkan/Managed/Fence.h"

namespace Engine::Vulkan {
    class Fence : public Managed::Fence {
    public:
        Fence(Ref<Device> device);
        virtual ~Fence();

    private:
        Ref<Device> m_Device;
    };
}  // namespace Engine::Vulkan