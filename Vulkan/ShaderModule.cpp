#include "ShaderModule.h"

namespace Vulkan {

    DEFAULT_MOVE_CONSTRUCTORS_IMPL(ShaderModule);

    Ref<ShaderModule> ShaderModule::Create(const std::vector<uint32_t>& shaderCode, VkDevice device) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderCode.size() * 4;
        createInfo.pCode    = shaderCode.data();

        VkShaderModule shaderModule;
        auto           res = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
        if (res != VK_SUCCESS) {
            return {};
        }
        return CreateRef<ShaderModule>(ShaderModule(shaderModule, device));
    }

    void ShaderModule::Swap(ShaderModule& other) {
        std::swap(m_Handle, other.m_Handle);
        std::swap(m_Device, other.m_Device);
    }

    ShaderModule::~ShaderModule() {
        if (m_Handle != VK_NULL_HANDLE) {
            vkDestroyShaderModule(m_Device, m_Handle, nullptr);
        }
    }

    ShaderModule::ShaderModule(VkShaderModule handle, VkDevice device) : m_Handle(handle), m_Device(device) {}

    VkShaderModule ShaderModule::Handle() {
        return m_Handle;
    }

}  // namespace Vulkan
