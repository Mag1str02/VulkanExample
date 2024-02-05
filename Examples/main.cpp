#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <Vulkan/Application.h>
#include <Vulkan/Helpers.h>
#include <Vulkan/ShaderModule.h>
#include <Vulkan/Utils/Assert.h>
#include <Vulkan/Window.h>

#include <thread>

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
        {
            // vkWaitForFences(m_Renderer->GetLogicDevice(), 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
            // vkResetFences(m_Renderer->GetLogicDevice(), 1, &m_InFlightFence);

            // uint32_t imageIndex;
            // vkAcquireNextImageKHR(
            //     m_Renderer->GetLogicDevice(), m_SwapChain->Handle(), UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

            // m_CommandBuffer->Reset();
            // RecordCommandBuffer(m_CommandBuffer, imageIndex);

            // VkSubmitInfo submitInfo{};
            // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            // VkSemaphore          signalSemaphores[] = {m_RenderFinishedSemaphore};
            // VkSemaphore          waitSemaphores[]   = {m_ImageAvailableSemaphore};
            // VkPipelineStageFlags waitStages[]       = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            // submitInfo.waitSemaphoreCount           = 1;
            // submitInfo.pWaitSemaphores              = waitSemaphores;
            // submitInfo.pWaitDstStageMask            = waitStages;
            // submitInfo.commandBufferCount           = 1;
            // submitInfo.pCommandBuffers              = &m_CommandBuffer->Handle();
            // submitInfo.signalSemaphoreCount         = 1;
            // submitInfo.pSignalSemaphores            = signalSemaphores;

            // vkQueueSubmit(m_Renderer->GetGraphicsQueue()->Handle(), 1, &submitInfo, m_InFlightFence);

            // VkPresentInfoKHR presentInfo{};
            // presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            // presentInfo.waitSemaphoreCount = 1;
            // presentInfo.pWaitSemaphores    = signalSemaphores;

            // VkSwapchainKHR swapChains[] = {m_SwapChain->Handle()};
            // presentInfo.swapchainCount  = 1;
            // presentInfo.pSwapchains     = swapChains;
            // presentInfo.pImageIndices   = &imageIndex;
            // presentInfo.pResults        = nullptr;  // Optional
            // vkQueuePresentKHR(m_Renderer->GetPresentationQueue()->Handle(), &presentInfo);
        }
        auto* wd = m_Window->ImGuiWindow();

        if (m_SwapChainRebuild) {
            int width, height;
            glfwGetFramebufferSize(m_Window->Handle(), &width, &height);
            if (width > 0 && height > 0) {
                ImGui_ImplVulkan_SetMinImageCount(2);
                ImGui_ImplVulkanH_CreateOrResizeWindow(m_Renderer->GetInstanceHandle(),
                                                       m_Renderer->GetPhysicalDevice(),
                                                       m_Renderer->GetLogicDevice(),
                                                       wd,
                                                       *m_Renderer->GetDevice()->GetFamilyIndex(QueueFamily::Graphics),
                                                       nullptr,
                                                       width,
                                                       height,
                                                       2);
                wd->FrameIndex     = 0;
                m_SwapChainRebuild = false;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("DockSpace",
                         NULL,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar |
                             ImGuiWindowFlags_NoDocking);
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
            ImGui::End();
        }

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
        // ImGui!).
        ImGui::ShowDemoWindow();

        ImGuiIO& io = ImGui::GetIO();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f       = 0.0f;
            static int   counter = 0;

            ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");  // Display some text (you can use a format strings too)

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color);  // Edit 3 floats representing a color

            if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImDrawData* main_draw_data      = ImGui::GetDrawData();
        const bool  main_is_minimized   = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
        wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
        wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
        wd->ClearValue.color.float32[3] = clear_color.w;
        if (!main_is_minimized) FrameRender(wd, main_draw_data);

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        FramePresent(wd);
    }

private:
    void FramePresent(ImGui_ImplVulkanH_Window* wd) {
        if (m_SwapChainRebuild) {
            return;
        }
        VkSemaphore      render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info                      = {};
        info.sType                                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount                    = 1;
        info.pWaitSemaphores                       = &render_complete_semaphore;
        info.swapchainCount                        = 1;
        info.pSwapchains                           = &wd->Swapchain;
        info.pImageIndices                         = &wd->FrameIndex;
        VkResult err                               = vkQueuePresentKHR(m_Renderer->GetGraphicsQueue()->Handle(), &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            m_SwapChainRebuild = true;
            return;
        }
        wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount;  // Now we can use the next set of semaphores
    }

    void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data) {
        auto     device = m_Renderer->GetLogicDevice();
        VkResult err;

        VkSemaphore image_acquired_semaphore  = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        err = vkAcquireNextImageKHR(device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
            m_SwapChainRebuild = true;
            return;
        }

        ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
        {
            VK_CHECK(vkWaitForFences(device, 1, &fd->Fence, VK_TRUE, UINT64_MAX));  // wait indefinitely instead of periodically checking

            VK_CHECK(vkResetFences(device, 1, &fd->Fence));
        }
        {
            VK_CHECK(vkResetCommandPool(device, fd->CommandPool, 0));
            VkCommandBufferBeginInfo info = {};
            info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VK_CHECK(vkBeginCommandBuffer(fd->CommandBuffer, &info));
        }
        {
            VkRenderPassBeginInfo info    = {};
            info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass               = wd->RenderPass;
            info.framebuffer              = fd->Framebuffer;
            info.renderArea.extent.width  = wd->Width;
            info.renderArea.extent.height = wd->Height;
            info.clearValueCount          = 1;
            info.pClearValues             = &wd->ClearValue;
            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(fd->CommandBuffer);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = 1;
            info.pWaitSemaphores            = &image_acquired_semaphore;
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &fd->CommandBuffer;
            info.signalSemaphoreCount       = 1;
            info.pSignalSemaphores          = &render_complete_semaphore;

            VK_CHECK(vkEndCommandBuffer(fd->CommandBuffer));
            VK_CHECK(vkQueueSubmit(m_Renderer->GetGraphicsQueue()->Handle(), 1, &info, fd->Fence));
        }
    }
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
        // commandBuffer->Begin();

        // Helpers::InsertImageMemoryBarier(commandBuffer->Handle(),
        //                                  m_SwapChain->GetImage(imageIndex),
        //                                  VK_IMAGE_LAYOUT_UNDEFINED,
        //                                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        //                                  VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        //                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

        // {
        //     VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        //     auto         extent     = m_SwapChain->GetExtent();

        //     VkRenderingAttachmentInfoKHR colorAttachmentInfo{};
        //     colorAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        //     colorAttachmentInfo.imageView   = m_SwapChain->GetImageView(imageIndex);
        //     colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        //     colorAttachmentInfo.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //     colorAttachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        //     colorAttachmentInfo.clearValue  = clearColor;

        //     VkRenderingInfoKHR renderInfo{};
        //     renderInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        //     renderInfo.renderArea.extent    = m_SwapChain->GetExtent();
        //     renderInfo.layerCount           = 1;
        //     renderInfo.colorAttachmentCount = 1;
        //     renderInfo.pColorAttachments    = &colorAttachmentInfo;

        //     vkCmdBeginRendering(commandBuffer->Handle(), &renderInfo);

        //     vkCmdBindPipeline(commandBuffer->Handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->Handle());

        //     m_CommandBuffer->SetViewport(extent.width, extent.height);
        //     m_CommandBuffer->SetScissor(extent.width, extent.height);
        //     m_CommandBuffer->Draw(3);

        //     vkCmdEndRendering(commandBuffer->Handle());
        // }

        // Helpers::InsertImageMemoryBarier(commandBuffer->Handle(),
        //                                  m_SwapChain->GetImage(imageIndex),
        //                                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        //                                  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        //                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        //                                  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        // m_CommandBuffer->End();
    }
    void CreatePipeline() {
        // auto triangleVertCode     = GetVertexShaderCode("Triangle");
        // auto triangleFragmentCode = GetFragmentShaderCode("Triangle");
        // DE_ASSERT(triangleVertCode.has_value(), "Failed");
        // DE_ASSERT(triangleFragmentCode.has_value(), "Failed");
        // m_TriangleVert = ShaderModule::Create(*triangleVertCode.value(), m_Renderer->GetLogicDevice());
        // m_TriangleFrag = ShaderModule::Create(*triangleFragmentCode.value(), m_Renderer->GetLogicDevice());
        // DE_ASSERT(m_TriangleVert, "Failed");
        // DE_ASSERT(m_TriangleFrag, "Failed");

        // PipelineSpecification spec;
        // spec.m_Stages.emplace(ShaderStage::Vertex, m_TriangleVert);
        // spec.m_Stages.emplace(ShaderStage::Fragment, m_TriangleFrag);
        // spec.m_Attachments.m_Colors.push_back(m_SwapChain->GetFormat());

        // m_Pipeline = m_Renderer->GetDevice()->CreatePipeline(spec);
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
    ImVec4      clear_color        = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool        m_SwapChainRebuild = false;
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