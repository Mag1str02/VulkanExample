#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"

namespace Vulkan {

    class ShaderModule {
    public:
        static Ref<ShaderModule> Create(const std::vector<uint32_t>& shaderCode, VkDevice device);
        ~ShaderModule();

        NO_COPY_CONSTRUCTORS(ShaderModule);
        MOVE_CONSTRUCTORS(ShaderModule);

        VkShaderModule Handle();

    private:
        ShaderModule(VkShaderModule handle, VkDevice device);
        void Swap(ShaderModule& other);

        VkShaderModule m_Handle = VK_NULL_HANDLE;
        VkDevice       m_Device = VK_NULL_HANDLE;
    };

}  // namespace Vulkan
