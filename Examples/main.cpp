#include <Vulkan/Application.h>
#include <Vulkan/ShaderModule.h>
#include <Vulkan/Utils/Assert.h>

#include "shaders_generated.h"

using namespace Vulkan;

class TestApplication : public Application {
public:
    TestApplication() = default;

    virtual void OnStartUp() override {
        CreateSyncObjects();
        CreatePipeline();
        CreateCommandBuffer();
    }
    virtual void OnShutDown() override {
        vkDeviceWaitIdle(m_Renderer->GetLogicDevice());
        vkDestroySemaphore(m_Renderer->GetLogicDevice(), m_ImageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_Renderer->GetLogicDevice(), m_RenderFinishedSemaphore, nullptr);
        vkDestroyFence(m_Renderer->GetLogicDevice(), m_InFlightFence, nullptr);
        vkDestroyCommandPool(m_Renderer->GetLogicDevice(), m_CommandPool, nullptr);
    }
    virtual void OnLoop() override {
        vkWaitForFences(m_Renderer->GetLogicDevice(), 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_Renderer->GetLogicDevice(), 1, &m_InFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(
            m_Renderer->GetLogicDevice(), m_SwapChain->Handle(), UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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

        vkQueueSubmit(m_Renderer->GetGraphicsQueue()->Handle(), 1, &submitInfo, m_InFlightFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_SwapChain->Handle()};
        presentInfo.swapchainCount  = 1;
        presentInfo.pSwapchains     = swapChains;
        presentInfo.pImageIndices   = &imageIndex;
        presentInfo.pResults        = nullptr;  // Optional
        vkQueuePresentKHR(m_Renderer->GetPresentationQueue()->Handle(), &presentInfo);
    }

private:
    void CreateSyncObjects() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        vkCreateSemaphore(m_Renderer->GetLogicDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
        vkCreateSemaphore(m_Renderer->GetLogicDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
        vkCreateFence(m_Renderer->GetLogicDevice(), &fenceInfo, nullptr, &m_InFlightFence);
    }
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        Helpers::InsertImageMemoryBarier(commandBuffer,
                                         m_SwapChain->GetImage(imageIndex),
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        {
            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

            VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
            colorAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            colorAttachmentInfo.imageView   = m_SwapChain->GetImageView(imageIndex);
            colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
            colorAttachmentInfo.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentInfo.clearValue  = clearColor;

            VkRenderingInfoKHR renderInfo{};
            renderInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
            renderInfo.renderArea.extent    = m_SwapChain->GetExtent();
            renderInfo.layerCount           = 1;
            renderInfo.colorAttachmentCount = 1;
            renderInfo.pColorAttachments    = &colorAttachmentInfo;

            vkCmdBeginRendering(commandBuffer, &renderInfo);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->Handle());
            VkViewport viewport{};
            viewport.x        = 0.0f;
            viewport.y        = 0.0f;
            viewport.width    = static_cast<float>(m_SwapChain->GetExtent().width);
            viewport.height   = static_cast<float>(m_SwapChain->GetExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = m_SwapChain->GetExtent();
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

            vkCmdEndRendering(commandBuffer);
        }

        Helpers::InsertImageMemoryBarier(commandBuffer,
                                         m_SwapChain->GetImage(imageIndex),
                                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                         VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        VK_CHECK(vkEndCommandBuffer(commandBuffer));
    }
    void CreatePipeline() {
        auto triangleVertCode     = GetVertexShaderCode("Triangle");
        auto triangleFragmentCode = GetFragmentShaderCode("Triangle");
        DE_ASSERT(triangleVertCode.has_value(), "Failed");
        DE_ASSERT(triangleFragmentCode.has_value(), "Failed");
        m_TriangleVert = ShaderModule::Create(*triangleVertCode.value(), m_Renderer->GetLogicDevice());
        m_TriangleFrag = ShaderModule::Create(*triangleFragmentCode.value(), m_Renderer->GetLogicDevice());
        DE_ASSERT(m_TriangleVert, "Failed");
        DE_ASSERT(m_TriangleFrag, "Failed");

        PipelineSpecification spec;
        spec.m_Stages.emplace(ShaderStage::Vertex, m_TriangleVert);
        spec.m_Stages.emplace(ShaderStage::Fragment, m_TriangleFrag);
        spec.m_Attachments.m_Colors.push_back(m_SwapChain->GetFormat());

        m_Pipeline = m_Renderer->GetDevice()->CreatePipeline(spec);
    }
    void CreateCommandBuffer() {
        {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = *(m_Renderer->GetDevice()->GetFamilyIndex(QueueFamily::Graphics));
            VK_CHECK(vkCreateCommandPool(m_Renderer->GetLogicDevice(), &poolInfo, nullptr, &m_CommandPool));
        }
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool        = m_CommandPool;
            allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            VK_CHECK(vkAllocateCommandBuffers(m_Renderer->GetLogicDevice(), &allocInfo, &m_CommandBuffer));
        }
    }

    Ref<ShaderModule> m_TriangleVert;
    Ref<ShaderModule> m_TriangleFrag;
    Ref<Pipeline>     m_Pipeline;

    VkCommandPool   m_CommandPool;
    VkCommandBuffer m_CommandBuffer;

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