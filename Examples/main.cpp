#include <Engine/Application/Application.h>
#include <Engine/Vulkan/Renderer.h>

#include <Engine/Vulkan/Concrete/CommandBuffer.h>
#include <Engine/Vulkan/Fence.h>
#include <Engine/Vulkan/ImageView.h>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

// #include "shaders_generated.h"

using namespace Engine;

class TestApplication : public Engine::Application {
public:
    TestApplication() = default;

    virtual void OnStartUp() override {
        auto command_pool = Vulkan::CommandPool::Create(m_Renderer->GetDevice(), m_Renderer->GetDevice()->GetQueue()->FamilyIndex());
        m_CommandBuffer   = Vulkan::Concrete::CommandBuffer::Create(command_pool);
    }
    virtual void OnShutDown() override {
        vkDeviceWaitIdle(m_Renderer->GetDevice()->GetLogicDevice());
    }
    virtual void OnLoop() override {
        auto image      = m_Window->GetSwapChainImage();
        auto image_view = CreateRef<Vulkan::ImageView>(image);
        m_CommandBuffer->Begin();

        {
            VkImageMemoryBarrier2 barrier{};
            barrier.image                           = image->Handle();
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_CLEAR_BIT;
            barrier.srcAccessMask                   = 0;
            barrier.dstAccessMask                   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.layerCount     = 1;
            barrier.subresourceRange.levelCount     = 1;
            m_CommandBuffer->AddImageMemoryBarier(barrier);
        }

        {
            float time = glfwGetTime();
            Vec4  clearValue;
            clearValue.r = (cos(time) + 1) / 2;
            clearValue.g = (sin(time) + 1) / 2;
            clearValue.b = 0;
            clearValue.a = 1;

            m_CommandBuffer->ClearImage(image, clearValue);
        }

        {
            VkImageMemoryBarrier2 barrier{};
            barrier.image                           = image->Handle();
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_CLEAR_BIT;
            barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            barrier.srcAccessMask                   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.layerCount     = 1;
            barrier.subresourceRange.levelCount     = 1;
            m_CommandBuffer->AddImageMemoryBarier(barrier);
        }
        m_CommandBuffer->BeginRendering({image_view});
        m_CommandBuffer->EndRendering();
        {
            VkImageMemoryBarrier2 barrier{};
            barrier.image                           = image->Handle();
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            barrier.srcAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.dstAccessMask                   = 0;
            barrier.oldLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.layerCount     = 1;
            barrier.subresourceRange.levelCount     = 1;

            m_CommandBuffer->AddImageMemoryBarier(barrier);
        }
        m_CommandBuffer->End();

        {
            VkCommandBuffer handle = m_CommandBuffer->Handle();

            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = 0;
            info.pWaitSemaphores            = nullptr;
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &handle;
            info.signalSemaphoreCount       = 0;
            info.pSignalSemaphores          = nullptr;
            Vulkan::Fence fence(m_Renderer->GetDevice());

            VK_CHECK(vkQueueSubmit(m_Renderer->GetQueue()->Handle(), 1, &info, fence.Handle()));
            fence.Wait();
        }
    }

private:
    Ref<Vulkan::Concrete::CommandBuffer> m_CommandBuffer;
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