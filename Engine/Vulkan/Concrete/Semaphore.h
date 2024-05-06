#pragma once

#include "Engine/Vulkan/Managed/Semaphore.h"

namespace Engine::Vulkan::Concrete {

    class Semaphore : public Managed::Semaphore {
    public:
        Semaphore(Ref<Device> device);
        virtual ~Semaphore();
    };

}  // namespace Engine::Vulkan::Concrete