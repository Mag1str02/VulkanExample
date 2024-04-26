#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Task.h"

namespace Engine::Vulkan::Interface {
    class SwapChain {
    public:
        virtual ~SwapChain() = default;

        virtual Ref<Task>   CreateAquireImageTask() = 0;
        virtual Ref<IImage> GetCurrentImage()       = 0;

        virtual VkExtent2D GetExtent() const = 0;
        virtual VkFormat   GetFormat() const = 0;
    };
}  // namespace Engine::Vulkan::Interface