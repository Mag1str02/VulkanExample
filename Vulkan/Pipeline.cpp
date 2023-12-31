#include "Pipeline.h"

#include "Vulkan/Device.h"
#include "Vulkan/ShaderModule.h"

namespace Vulkan {

    namespace {
        VkShaderStageFlagBits GetShaderStage(ShaderStage stage) {
            switch (stage) {
                case ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
                case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
                case ShaderStage::None: DE_ASSERT(false, "None shader stage"); return VK_SHADER_STAGE_ALL;
                default: DE_ASSERT(false, "Unknown shader stage"); return VK_SHADER_STAGE_ALL;
            }
        }

        VkPipelineShaderStageCreateInfo GenShaderStageStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module) {
            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage  = stage;
            shaderStageInfo.module = module;
            shaderStageInfo.pName  = "main";
            return shaderStageInfo;
        }

    }  // namespace

    Pipeline::Pipeline(Ref<Device> device, const PipelineSpecification& spec) {
        DE_ASSERT(device, "Bad device");
        m_Specification = spec;
        m_Device        = device;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // TODO: pass info about layouts
        pipelineLayoutInfo.setLayoutCount         = 0;        // Optional
        pipelineLayoutInfo.pSetLayouts            = nullptr;  // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;        // Optional
        pipelineLayoutInfo.pPushConstantRanges    = nullptr;  // Optional

        VK_CHECK(vkCreatePipelineLayout(m_Device->GetLogicDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout));

        std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfos;
        for (const auto& [stage, module] : m_Specification.m_Stages) {
            DE_ASSERT(module, "Bad module");
            auto vkStage = GetShaderStage(stage);
            stageCreateInfos.push_back(GenShaderStageStageCreateInfo(vkStage, module->Handle()));
        }

        VkPipelineVertexInputStateCreateInfo   vertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        VkPipelineViewportStateCreateInfo      viewportState{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo   multisampling{};
        VkPipelineColorBlendAttachmentState    colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo    colorBlending{};
        VkPipelineDynamicStateCreateInfo       dynamicState{};
        VkPipelineRenderingCreateInfoKHR       renderingCreateInfo{};

        {
            vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount   = 0;
            vertexInputInfo.pVertexBindingDescriptions      = nullptr;  // Optional
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions    = nullptr;  // Optional
            // TODO: pass info about buffers and attributes

            inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount  = 1;

            rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable        = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth               = 1.0f;
            rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable         = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f;  // Optional
            rasterizer.depthBiasClamp          = 0.0f;  // Optional
            rasterizer.depthBiasSlopeFactor    = 0.0f;  // Optional

            multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable   = VK_FALSE;
            multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading      = 1.0f;      // Optional
            multisampling.pSampleMask           = nullptr;   // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
            multisampling.alphaToOneEnable      = VK_FALSE;  // Optional

            colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable         = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

            colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable     = VK_FALSE;
            colorBlending.logicOp           = VK_LOGIC_OP_COPY;  // Optional
            colorBlending.attachmentCount   = 1;
            colorBlending.pAttachments      = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f;  // Optionals
            colorBlending.blendConstants[1] = 0.0f;  // Optional
            colorBlending.blendConstants[2] = 0.0f;  // Optional
            colorBlending.blendConstants[3] = 0.0f;  // Optional

            std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
            dynamicState.sType                        = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount            = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates               = dynamicStates.data();

            renderingCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
            renderingCreateInfo.colorAttachmentCount    = m_Specification.m_Attachments.m_Colors.size();
            renderingCreateInfo.pColorAttachmentFormats = m_Specification.m_Attachments.m_Colors.data();
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount          = stageCreateInfos.size();
        pipelineInfo.pStages             = stageCreateInfos.data();
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState   = &multisampling;
        pipelineInfo.pDepthStencilState  = nullptr;  // Optional
        pipelineInfo.pColorBlendState    = &colorBlending;
        pipelineInfo.pDynamicState       = &dynamicState;
        pipelineInfo.layout              = m_PipelineLayout;
        pipelineInfo.renderPass          = VK_NULL_HANDLE;
        pipelineInfo.subpass             = 0;
        pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;  // Optional
        pipelineInfo.basePipelineIndex   = -1;              // Optional

        VK_CHECK(vkCreateGraphicsPipelines(m_Device->GetLogicDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline));
    }

    const VkPipeline& Pipeline::Handle() {
        return m_Pipeline;
    }

    Pipeline::~Pipeline() {
        vkDestroyPipeline(m_Device->GetLogicDevice(), m_Pipeline, nullptr);
        vkDestroyPipelineLayout(m_Device->GetLogicDevice(), m_PipelineLayout, nullptr);
    }

}  // namespace Vulkan