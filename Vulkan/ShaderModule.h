#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"

class ShaderModule {
public:
    static Result<Ref<ShaderModule>> Create(const std::vector<uint32_t>& shaderCode, VkDevice device);

    ShaderModule(ShaderModule&& other);
    ShaderModule& operator=(ShaderModule&& other);

    ShaderModule(const ShaderModule& other)            = delete;
    ShaderModule& operator=(const ShaderModule& other) = delete;

    ~ShaderModule();

    VkPipelineShaderStageCreateInfo GetShaderStage(VkShaderStageFlagBits stage);

private:
    ShaderModule(VkShaderModule handle, VkDevice device);
    void Destroy();

    VkShaderModule        m_Handle = VK_NULL_HANDLE;
    VkDevice              m_Device = VK_NULL_HANDLE;
};