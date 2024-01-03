#include <Vulkan/Utils/Assert.h>
#include <Vulkan/ShaderModule.h>
#include <Vulkan/Application.h>

#include "shaders_generated.h"

using namespace Vulkan;

class TestApplication : public Application {
public:
    TestApplication() = default;

    virtual void OnStartUp() override {
        CreateRenderPass();
        CreatePipelineLayout();
        CreatePipeline();
        CreateFramebuffers();
        CreateSyncObjects();
    }
    virtual void OnShutDown() override {
        vkDeviceWaitIdle(m_LogicDevice);
        vkDestroySemaphore(m_LogicDevice, m_ImageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_LogicDevice, m_RenderFinishedSemaphore, nullptr);
        vkDestroyFence(m_LogicDevice, m_InFlightFence, nullptr);
        vkDestroyCommandPool(m_LogicDevice, m_CommandPool, nullptr);

        for (auto framebuffer : m_SwapChainFramebuffers) {
            vkDestroyFramebuffer(m_LogicDevice, framebuffer, nullptr);
        }

        vkDestroyPipeline(m_LogicDevice, m_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_LogicDevice, m_PipelineLayout, nullptr);
        vkDestroyRenderPass(m_LogicDevice, m_RenderPass, nullptr);
    }
    virtual void OnLoop() override {
        vkWaitForFences(m_LogicDevice, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_LogicDevice, 1, &m_InFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_LogicDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
        vkResetCommandBuffer(m_CommandBuffer, 0);
        RecordCommandBuffer(m_CommandBuffer, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore          signalSemaphores[] = {m_RenderFinishedSemaphore};
        VkSemaphore          waitSemaphores[]   = {m_ImageAvailableSemaphore};
        VkPipelineStageFlags waitStages[]       = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount           = 1;
        submitInfo.pWaitSemaphores              = waitSemaphores;
        submitInfo.pWaitDstStageMask            = waitStages;
        submitInfo.commandBufferCount           = 1;
        submitInfo.pCommandBuffers              = &m_CommandBuffer;
        submitInfo.signalSemaphoreCount         = 1;
        submitInfo.pSignalSemaphores            = signalSemaphores;

        vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_SwapChain};
        presentInfo.swapchainCount  = 1;
        presentInfo.pSwapchains     = swapChains;
        presentInfo.pImageIndices   = &imageIndex;
        presentInfo.pResults        = nullptr;  // Optional
        vkQueuePresentKHR(m_PresentQueue, &presentInfo);
    }

private:
    void CreateSyncObjects() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        vkCreateSemaphore(m_LogicDevice, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
        vkCreateSemaphore(m_LogicDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
        vkCreateFence(m_LogicDevice, &fenceInfo, nullptr, &m_InFlightFence);
    }
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags            = 0;        // Optional
        beginInfo.pInheritanceInfo = nullptr;  // Optional

        if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = m_RenderPass;
        renderPassInfo.framebuffer       = m_SwapChainFramebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_SwapChainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues    = &clearColor;
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
        VkViewport viewport{};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<float>(m_SwapChainExtent.width);
        viewport.height   = static_cast<float>(m_SwapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_SwapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }
    void CreateFramebuffers() {
        for (const auto& imageView : m_SwapChainImageViews) {
            std::vector<VkImageView> attachments = {imageView};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass      = m_RenderPass;
            framebufferInfo.attachmentCount = attachments.size();
            framebufferInfo.pAttachments    = attachments.data();
            framebufferInfo.width           = m_SwapChainExtent.width;
            framebufferInfo.height          = m_SwapChainExtent.height;
            framebufferInfo.layers          = 1;

            VkFramebuffer frameBuffer;
            auto          res = vkCreateFramebuffer(m_LogicDevice, &framebufferInfo, nullptr, &frameBuffer);
            DE_ASSERT(res == VK_SUCCESS, "Failed to create framebuffer!");
            m_SwapChainFramebuffers.push_back(frameBuffer);
        }
        {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = m_GraphicsQueueFamilyIndex;
            auto res                  = vkCreateCommandPool(m_LogicDevice, &poolInfo, nullptr, &m_CommandPool);
            DE_ASSERT(res == VK_SUCCESS, "failed to create command pool!");
        }
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool        = m_CommandPool;
            allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            auto res = vkAllocateCommandBuffers(m_LogicDevice, &allocInfo, &m_CommandBuffer);
            DE_ASSERT(res == VK_SUCCESS, "Failed to allocate command buffer!");
        }
    }
    void CreatePipeline() {
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        stages.push_back(m_TriangleVert->GetShaderStage(VK_SHADER_STAGE_VERTEX_BIT));
        stages.push_back(m_TriangleFrag->GetShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT));

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount   = 0;
        vertexInputInfo.pVertexBindingDescriptions      = nullptr;  // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions    = nullptr;  // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
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

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading      = 1.0f;      // Optional
        multisampling.pSampleMask           = nullptr;   // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
        multisampling.alphaToOneEnable      = VK_FALSE;  // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable         = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable     = VK_FALSE;
        colorBlending.logicOp           = VK_LOGIC_OP_COPY;  // Optional
        colorBlending.attachmentCount   = 1;
        colorBlending.pAttachments      = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;  // Optionals
        colorBlending.blendConstants[1] = 0.0f;  // Optional
        colorBlending.blendConstants[2] = 0.0f;  // Optional
        colorBlending.blendConstants[3] = 0.0f;  // Optional

        std::vector<VkDynamicState>      dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates    = dynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount          = stages.size();
        pipelineInfo.pStages             = stages.data();
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState   = &multisampling;
        pipelineInfo.pDepthStencilState  = nullptr;  // Optional
        pipelineInfo.pColorBlendState    = &colorBlending;
        pipelineInfo.pDynamicState       = &dynamicState;
        pipelineInfo.layout              = m_PipelineLayout;
        pipelineInfo.renderPass          = m_RenderPass;
        pipelineInfo.subpass             = 0;
        pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;  // Optional
        pipelineInfo.basePipelineIndex   = -1;              // Optional

        auto res = vkCreateGraphicsPipelines(m_LogicDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create graphics pipeline");
    }
    void CreateRenderPass() {
        VkSubpassDependency dependency{};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format         = m_SwapChainImageFormat;
        colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments    = &colorAttachment;
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        auto res = vkCreateRenderPass(m_LogicDevice, &renderPassInfo, nullptr, &m_RenderPass);
        DE_ASSERT(res == VK_SUCCESS, "failed to create render pass!");
    }
    void CreatePipelineLayout() {
        auto triangleVertCode     = GetVertexShaderCode("Triangle");
        auto triangleFragmentCode = GetFragmentShaderCode("Triangle");
        DE_ASSERT(triangleVertCode.has_value(), "Failed");
        DE_ASSERT(triangleFragmentCode.has_value(), "Failed");
        auto triangleVert = ShaderModule::Create(*triangleVertCode.value(), m_LogicDevice);
        auto triangleFrag = ShaderModule::Create(*triangleFragmentCode.value(), m_LogicDevice);
        DE_ASSERT(triangleVert.has_value(), "Failed");
        DE_ASSERT(triangleFrag.has_value(), "Failed");
        m_TriangleVert = triangleVert.value();
        m_TriangleFrag = triangleFrag.value();

        VkViewport viewport{};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = (float)m_SwapChainExtent.width;
        viewport.height   = (float)m_SwapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_SwapChainExtent;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount         = 0;        // Optional
        pipelineLayoutInfo.pSetLayouts            = nullptr;  // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;        // Optional
        pipelineLayoutInfo.pPushConstantRanges    = nullptr;  // Optional

        auto res = vkCreatePipelineLayout(m_LogicDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create pipeline layout");
    }

    Ref<ShaderModule> m_TriangleVert;
    Ref<ShaderModule> m_TriangleFrag;

    VkRenderPass               m_RenderPass;
    VkPipelineLayout           m_PipelineLayout;
    VkPipeline                 m_GraphicsPipeline;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;
    VkCommandPool              m_CommandPool;
    VkCommandBuffer            m_CommandBuffer;

    VkSemaphore m_ImageAvailableSemaphore;
    VkSemaphore m_RenderFinishedSemaphore;
    VkFence     m_InFlightFence;
};

int main() {
    {
        TestApplication app;
        app.Run();
    }
    {
        TestApplication app;
        app.Run();
    }
    return 0;
}