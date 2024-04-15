#include "ImageSampler.h"

#include "Device.h"

namespace Engine::Vulkan {

    ImageSampler::ImageSampler(Ref<Device> device) : m_Device(device) {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter    = VK_FILTER_NEAREST;
        samplerInfo.minFilter    = VK_FILTER_NEAREST;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.borderColor  = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

        samplerInfo.anisotropyEnable        = VK_FALSE;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable           = VK_FALSE;
        samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias              = 0.0f;
        samplerInfo.minLod                  = 0.0f;
        samplerInfo.maxLod                  = 0.0f;

        VK_CHECK(vkCreateSampler(m_Device->GetLogicDevice(), &samplerInfo, nullptr, &m_Sampler));
    }
    ImageSampler::~ImageSampler() {
        vkDestroySampler(m_Device->GetLogicDevice(), m_Sampler, nullptr);
    }

    VkSampler ImageSampler::Handle() {
        return m_Sampler;
    }

}  // namespace Vulkan