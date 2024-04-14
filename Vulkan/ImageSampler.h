#pragma once

#include "Common.h"

namespace Vulkan {

    class ImageSampler {
    public:
        ImageSampler(Ref<Device> device);
        ~ImageSampler();

        VkSampler Handle();

    private:
        Ref<Device> m_Device;
        VkSampler   m_Sampler;
    };
}  // namespace Vulkan