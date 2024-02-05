#pragma once

#include <volk.h>

#include "Vulkan/Base.h"

namespace Vulkan {

    class ShaderModule {
    public:
        static Ref<ShaderModule> Create(const std::vector<uint32_t>& shaderCode, VkDevice device);
        ~ShaderModule();

        const VkShaderModule& Handle();

        NO_COPY_CONSTRUCTORS(ShaderModule);
        MOVE_CONSTRUCTORS(ShaderModule);

    private:
        ShaderModule(VkShaderModule handle, VkDevice device);
        void Swap(ShaderModule& other);

    private:
        VkShaderModule m_Handle = VK_NULL_HANDLE;
        VkDevice       m_Device = VK_NULL_HANDLE;
    };

}  // namespace Vulkan
