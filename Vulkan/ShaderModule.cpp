#include "ShaderModule.h"

Result<Ref<ShaderModule>> ShaderModule::Create(const fs::path& path, VkDevice device) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return std::unexpected("Failed to open shader file: " + path.string());
    }

    size_t fileSize = (size_t)file.tellg();

    std::vector<char> byteCode(fileSize);
    file.seekg(0);
    file.read(byteCode.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = byteCode.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(byteCode.data());

    VkShaderModule shaderModule;
    auto           res = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    if (res != VK_SUCCESS) {
        return std::unexpected("Failed to create shader module. Error code: " + std::to_string(res));
    }
    return CreateRef<ShaderModule>(ShaderModule(shaderModule, device));
}

ShaderModule::ShaderModule(ShaderModule&& other) {
    std::swap(m_Handle, other.m_Handle);
    std::swap(m_Device, other.m_Device);
}
ShaderModule& ShaderModule::operator=(ShaderModule&& other) {
    if (&other == this) {
        return *this;
    }
    Destroy();
    std::swap(m_Handle, other.m_Handle);
    std::swap(m_Device, other.m_Device);
    return *this;
}
VkPipelineShaderStageCreateInfo ShaderModule::GetShaderStage(VkShaderStageFlagBits stage) {
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage  = stage;
    shaderStageInfo.module = m_Handle;
    shaderStageInfo.pName  = "main";
    return shaderStageInfo;
}

ShaderModule::~ShaderModule() {
    Destroy();
}

void ShaderModule::Destroy() {
    if (m_Handle != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_Device, m_Handle, nullptr);
    }
    m_Handle = VK_NULL_HANDLE;
    m_Device = VK_NULL_HANDLE;
}

ShaderModule::ShaderModule(VkShaderModule handle, VkDevice device) : m_Handle(handle), m_Device(device) {}
