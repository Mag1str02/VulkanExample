#include "Window.h"

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Helpers.h"
#include "ImageView.h"
#include "Queue.h"
#include "Renderer.h"
#include "SwapChain.h"

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace {
    static void check_vk_result(VkResult err) {
        if (err == 0) return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0) abort();
    }

    static constexpr uint32_t k_MinImageCount = 3;

}  // namespace

namespace Engine::Vulkan {

    Window::Window(Renderer* renderer) : m_Renderer(renderer) {
        VK_CHECK(glfwCreateWindowSurface(m_Renderer->GetInstance()->Handle(), m_WindowHandle, nullptr, &m_Surface));
        auto size       = GetSize();
        m_SwapChain     = SwapChain::Create(m_Surface, m_Renderer->GetDevice(), VkExtent2D{.width = size.x, .height = size.y});
        m_CommandBuffer = CommandPool::Create(m_Renderer->GetDevice(), m_Renderer->GetDevice()->GetQueue()->FamilyIndex())->CreateCommandBuffer();
    }

    Window::~Window() {
        m_SwapChain.reset();
        vkDestroySurfaceKHR(m_Renderer->GetInstance()->Handle(), m_Surface, nullptr);
    }

    void Window::BeginFrame() {
        Engine::Window::BeginFrame();
    }
    void Window::EndFrame() {
        Ref<IImage>    image      = m_SwapChain->AquireNextImage();
        Ref<ImageView> image_view = CreateRef<ImageView>(image);
        m_CommandBuffer->Begin();
        {
            VkImageMemoryBarrier2 barrier{};
            barrier.image                           = image->Handle();
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            barrier.srcAccessMask                   = 0;
            barrier.dstAccessMask                   = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout                       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.layerCount     = 1;
            barrier.subresourceRange.levelCount     = 1;

            VkDependencyInfo dependencyInfo{};
            dependencyInfo.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            dependencyInfo.imageMemoryBarrierCount = 1;
            dependencyInfo.pImageMemoryBarriers    = &barrier;

            vkCmdPipelineBarrier2(m_CommandBuffer->Handle(), &dependencyInfo);
        }
        {
            float        time = glfwGetTime();
            VkClearValue clearValue{};
            clearValue.color.float32[0] = (cos(time) + 1) / 2;
            clearValue.color.float32[1] = (sin(time) + 1) / 2;
            clearValue.color.float32[2] = 0;
            clearValue.color.float32[3] = 1;

            VkRenderingAttachmentInfo attachment{};
            attachment.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachment.imageView   = image_view->Handle();
            attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
            attachment.clearValue  = clearValue;

            VkRenderingInfo info{};
            info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
            info.renderArea.extent    = image->GetExtent();
            info.layerCount           = 1;
            info.colorAttachmentCount = 1;
            info.pColorAttachments    = &attachment;

            vkCmdBeginRendering(m_CommandBuffer->Handle(), &info);
        }

        vkCmdEndRendering(m_CommandBuffer->Handle());
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

            VkDependencyInfo dependencyInfo{};
            dependencyInfo.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            dependencyInfo.imageMemoryBarrierCount = 1;
            dependencyInfo.pImageMemoryBarriers    = &barrier;

            vkCmdPipelineBarrier2(m_CommandBuffer->Handle(), &dependencyInfo);
        }

        m_CommandBuffer->End();

        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = 0;
            info.pWaitSemaphores            = nullptr;
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &m_CommandBuffer->Handle();
            info.signalSemaphoreCount       = 0;
            info.pSignalSemaphores          = nullptr;
            Fence fence(m_Renderer->GetDevice());

            VK_CHECK(vkQueueSubmit(m_Renderer->GetQueue()->Handle(), 1, &info, fence.Handle()));
            fence.Wait();
        }

        Engine::Window::EndFrame();
    }

    VkSurfaceKHR Window::GetSurface() {
        return m_Surface;
    }

}  // namespace Engine::Vulkan