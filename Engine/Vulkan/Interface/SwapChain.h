#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Task.h"

namespace Engine::Vulkan::Interface {
    class SwapChain : public NonCopyMoveable {
    public:
        class PresentAquireTask : public Task {
        public:
            virtual Ref<IImage> GetAquiredImage() const = 0;
        };

        virtual ~SwapChain() = default;

        virtual Ref<PresentAquireTask> CreateAquireImageTask() = 0;

        virtual VkExtent2D GetExtent() const = 0;
        virtual VkFormat   GetFormat() const = 0;
    };
}  // namespace Engine::Vulkan::Interface