#include <Vulkan/Application.h>
#include <Vulkan/Helpers.h>
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
    }
    virtual void OnLoop() override {
        vkWaitForFences(m_Renderer->GetLogicDevice(), 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_Renderer->GetLogicDevice(), 1, &m_InFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(
            m_Renderer->GetLogicDevice(), m_SwapChain->Handle(), UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        m_CommandBuffer->Reset();
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
        submitInfo.pCommandBuffers              = &m_CommandBuffer->Handle();
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
    void RecordCommandBuffer(Ref<CommandBuffer> commandBuffer, uint32_t imageIndex) {
        commandBuffer->Begin();

        Helpers::InsertImageMemoryBarier(commandBuffer->Handle(),
                                         m_SwapChain->GetImage(imageIndex),
                                         VK_IMAGE_LAYOUT_UNDEFINED,
                                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        {
            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
            auto         extent     = m_SwapChain->GetExtent();

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

            vkCmdBeginRendering(commandBuffer->Handle(), &renderInfo);

            vkCmdBindPipeline(commandBuffer->Handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->Handle());

            m_CommandBuffer->SetViewport(extent.width, extent.height);
            m_CommandBuffer->SetScissor(extent.width, extent.height);
            m_CommandBuffer->Draw(3);

            vkCmdEndRendering(commandBuffer->Handle());
        }

        Helpers::InsertImageMemoryBarier(commandBuffer->Handle(),
                                         m_SwapChain->GetImage(imageIndex),
                                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                         VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        m_CommandBuffer->End();
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
        auto m_CommandPool = m_Renderer->GetDevice()->CreateCommandPool(QueueFamily::Graphics);
        m_CommandBuffer    = m_CommandPool->CreateCommandBuffer();
    }

    Ref<ShaderModule> m_TriangleVert;
    Ref<ShaderModule> m_TriangleFrag;
    Ref<Pipeline>     m_Pipeline;

    Ref<CommandBuffer> m_CommandBuffer;

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