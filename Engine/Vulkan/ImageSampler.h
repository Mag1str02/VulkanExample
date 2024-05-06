#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class ImageSampler : NonCopyMoveable {
    public:
        ImageSampler(Ref<Device> device);
        ~ImageSampler();

        VkSampler Handle();

    private:
        Ref<Device> m_Device;
        VkSampler   m_Sampler;
    };
}  // namespace Engine::Vulkan