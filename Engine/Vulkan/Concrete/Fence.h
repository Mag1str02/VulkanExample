#pragma once

#include "Engine/Vulkan/Managed/Fence.h"

namespace Engine::Vulkan::Concrete {
    class Fence : public Managed::Fence {
    public:
        Fence(Ref<Device> device);
        virtual ~Fence();
    };
}  // namespace Engine::Vulkan::Concrete