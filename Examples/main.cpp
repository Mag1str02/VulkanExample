#include <Engine/Application/Application.h>
#include <Engine/Utils/Assert.h>
#include <Engine/Vulkan/Buffer.h>
#include <Engine/Vulkan/Helpers.h>
#include <Engine/Vulkan/Image.h>
#include <Engine/Vulkan/ImageSampler.h>
#include <Engine/Vulkan/ImageView.h>
#include <Engine/Vulkan/Window.h>

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include "shaders_generated.h"

using namespace Engine;

class TestApplication : public Engine::Application {
public:
    TestApplication() = default;

    virtual void OnStartUp() override {
        // m_CommandPool = m_Renderer->GetDevice()->CreateCommandPool(Engine::Vulkan::QueueFamily::Graphics);
        // CreateTexture();
    }
    virtual void OnShutDown() override {
        vkDeviceWaitIdle(m_Renderer->GetDevice()->GetLogicDevice());
    }
    virtual void OnLoop() override {
        // OnImGui();
    }

private:
    // void CreateTexture() {
    //     // Create iamges
    //     {
    //         uint8_t image[] = {255, 0, 0, 255, 0, 255, 0, 255};

    //         m_StagingBuffer = CreateRef<Vulkan::Buffer>(m_Renderer->GetDevice(),
    //                                                     sizeof(image),
    //                                                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    //                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    //         m_StagingBuffer->SetData(image, sizeof(image));

    //         m_Image        = CreateRef<Vulkan::Image>(m_Renderer->GetDevice(),
    //                                            2,
    //                                            1,
    //                                            VK_FORMAT_R8G8B8A8_SRGB,
    //                                            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    //                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    //         m_ImageSampler = CreateRef<Vulkan::ImageSampler>(m_Renderer->GetDevice());
    //         m_ImageView    = CreateRef<Vulkan::ImageView>(m_Image);
    //     }

    //     // Transfer texture data
    //     {
    //         auto queue = m_Renderer->GetGraphicsQueue();
    //         auto cmd   = m_CommandPool->CreateCommandBuffer();

    //         cmd->Begin();
    //         // First image layout transition
    //         {
    //             VkImageMemoryBarrier2 barrier{};
    //             barrier.image                           = m_Image->Handle();
    //             barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    //             barrier.srcStageMask                    = 0;
    //             barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    //             barrier.srcAccessMask                   = 0;
    //             barrier.dstAccessMask                   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    //             barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
    //             barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //             barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    //             barrier.subresourceRange.baseArrayLayer = 0;
    //             barrier.subresourceRange.baseMipLevel   = 0;
    //             barrier.subresourceRange.layerCount     = 1;
    //             barrier.subresourceRange.levelCount     = 1;

    //             VkDependencyInfo dependencyInfo{};
    //             dependencyInfo.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    //             dependencyInfo.imageMemoryBarrierCount = 1;
    //             dependencyInfo.pImageMemoryBarriers    = &barrier;

    //             vkCmdPipelineBarrier2(cmd->Handle(), &dependencyInfo);
    //         }
    //         // Transfer image data from staging buffer to image
    //         {
    //             VkBufferImageCopy region{};
    //             region.bufferOffset      = 0;
    //             region.bufferRowLength   = 0;
    //             region.bufferImageHeight = 0;

    //             region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    //             region.imageSubresource.mipLevel       = 0;
    //             region.imageSubresource.baseArrayLayer = 0;
    //             region.imageSubresource.layerCount     = 1;

    //             region.imageOffset = {0, 0, 0};
    //             region.imageExtent = {2, 1, 1};
    //             vkCmdCopyBufferToImage(cmd->Handle(), m_StagingBuffer->Handle(), m_Image->Handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
    //             &region);
    //         }
    //         // Second image layout transition
    //         {
    //             VkImageMemoryBarrier2 barrier{};
    //             barrier.image                           = m_Image->Handle();
    //             barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    //             barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    //             barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    //             barrier.srcAccessMask                   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    //             barrier.dstAccessMask                   = VK_ACCESS_2_SHADER_READ_BIT;
    //             barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //             barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    //             barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    //             barrier.subresourceRange.baseArrayLayer = 0;
    //             barrier.subresourceRange.baseMipLevel   = 0;
    //             barrier.subresourceRange.layerCount     = 1;
    //             barrier.subresourceRange.levelCount     = 1;

    //             VkDependencyInfo dependencyInfo{};
    //             dependencyInfo.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    //             dependencyInfo.imageMemoryBarrierCount = 1;
    //             dependencyInfo.pImageMemoryBarriers    = &barrier;

    //             vkCmdPipelineBarrier2(cmd->Handle(), &dependencyInfo);
    //         }
    //         cmd->End();

    //         VkSubmitInfo submitInfo{};
    //         submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //         submitInfo.commandBufferCount = 1;
    //         submitInfo.pCommandBuffers    = &cmd->Handle();

    //         vkQueueSubmit(queue->Handle(), 1, &submitInfo, VK_NULL_HANDLE);
    //         vkQueueWaitIdle(queue->Handle());
    //     }

    //     m_ImGuiTextureID = ImGui_ImplVulkan_AddTexture(m_ImageSampler->Handle(), m_ImageView->Handle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // }

    void OnImGui() {
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

        ImGui::ShowDemoWindow();

        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Begin("Hello, world!");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Image(m_ImGuiTextureID, {200, 100});
            ImGui::End();
        }
    }

    Ref<Vulkan::Buffer>       m_StagingBuffer;
    Ref<Vulkan::Image>        m_Image;
    Ref<Vulkan::CommandPool>  m_CommandPool;
    Ref<Vulkan::ImageView>    m_ImageView;
    Ref<Vulkan::ImageSampler> m_ImageSampler;
    VkDescriptorSet           m_ImGuiTextureID;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
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