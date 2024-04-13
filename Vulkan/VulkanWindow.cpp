#include "VulkanWindow.h"

#include "Vulkan/Device.h"
#include "Vulkan/Helpers.h"
#include "Vulkan/Queue.h"
#include "Vulkan/Renderer.h"

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

namespace Vulkan {
    Window::Window(Ref<Renderer> renderer) {
        DE_ASSERT(renderer, "Bad renderer");
        m_Renderer = renderer;

        { VK_CHECK(glfwCreateWindowSurface(m_Renderer->GetInstanceHandle(), Handle(), nullptr, &m_Surface)); }

        {
            m_ImGuiWindow          = CreateRef<ImGui_ImplVulkanH_Window>();
            m_ImGuiWindow->Surface = m_Surface;

            const VkFormat        requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM,
                                                                 VK_FORMAT_R8G8B8A8_UNORM,
                                                                 VK_FORMAT_B8G8R8_UNORM,
                                                                 VK_FORMAT_R8G8B8_UNORM};
            const VkColorSpaceKHR requestSurfaceColorSpace    = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
            m_ImGuiWindow->SurfaceFormat                      = ImGui_ImplVulkanH_SelectSurfaceFormat(m_Renderer->GetPhysicalDevice(),
                                                                                 m_Surface,
                                                                                 requestSurfaceImageFormat,
                                                                                 (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat),
                                                                                 requestSurfaceColorSpace);

            VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
            m_ImGuiWindow->PresentMode =
                ImGui_ImplVulkanH_SelectPresentMode(m_Renderer->GetPhysicalDevice(), m_Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));

            RefreshSwapChain();
        }
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows
        }
        {
            VkDescriptorPoolSize pool_sizes[] = {
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
            };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets                    = 1000;
            pool_info.poolSizeCount              = (uint32_t)IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes                 = pool_sizes;
            VK_CHECK(vkCreateDescriptorPool(m_Renderer->GetLogicDevice(), &pool_info, nullptr, &m_DescriptorPool));
        }
        {
            ImGui_ImplGlfw_InitForVulkan(Handle(), true);
            ImGui_ImplVulkan_InitInfo init_info = {};
            init_info.Instance                  = m_Renderer->GetInstanceHandle();
            init_info.PhysicalDevice            = m_Renderer->GetPhysicalDevice();
            init_info.Device                    = m_Renderer->GetLogicDevice();
            init_info.QueueFamily               = *m_Renderer->GetDevice()->GetFamilyIndex(Vulkan::QueueFamily::Graphics);
            init_info.Queue                     = m_Renderer->GetDevice()->GetQueue(Vulkan::QueueFamily::Graphics, 0)->Handle();
            init_info.PipelineCache             = VK_NULL_HANDLE;
            init_info.DescriptorPool            = m_DescriptorPool;
            init_info.Subpass                   = 0;
            init_info.MinImageCount             = k_MinImageCount;
            init_info.ImageCount                = m_ImGuiWindow->ImageCount;
            init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
            init_info.Allocator                 = nullptr;
            init_info.CheckVkResultFn           = check_vk_result;
            ImGui_ImplVulkan_Init(&init_info, m_ImGuiWindow->RenderPass);
        }
    }

    Window::~Window() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        vkDestroyDescriptorPool(m_Renderer->GetLogicDevice(), m_DescriptorPool, nullptr);
        ImGui::DestroyContext();
        ImGui_ImplVulkanH_DestroyWindow(m_Renderer->GetInstanceHandle(), m_Renderer->GetLogicDevice(), m_ImGuiWindow.get(), nullptr);
        m_ImGuiWindow = nullptr;
    }

    ImGui_ImplVulkanH_Window* Window::GetImGuiWindow() {
        return m_ImGuiWindow.get();
    }

    void Window::RefreshSwapChain() {
        if (!m_SwapChainValid) {
            auto [width, height] = GetSize();
            if (width > 0 && height > 0) {
                ImGui_ImplVulkanH_CreateOrResizeWindow(m_Renderer->GetInstanceHandle(),
                                                       m_Renderer->GetPhysicalDevice(),
                                                       m_Renderer->GetLogicDevice(),
                                                       m_ImGuiWindow.get(),
                                                       *m_Renderer->GetDevice()->GetFamilyIndex(QueueFamily::Graphics),
                                                       nullptr,
                                                       width,
                                                       height,
                                                       k_MinImageCount);
                m_ImGuiWindow->FrameIndex = 0;
                m_SwapChainValid          = true;
            }
        }
    }
    void Window::RenderFrame() {
        // Render Frame
        {
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

            auto     device = m_Renderer->GetLogicDevice();
            VkResult err;

            VkSemaphore image_acquired_semaphore  = m_ImGuiWindow->FrameSemaphores[m_ImGuiWindow->SemaphoreIndex].ImageAcquiredSemaphore;
            VkSemaphore render_complete_semaphore = m_ImGuiWindow->FrameSemaphores[m_ImGuiWindow->SemaphoreIndex].RenderCompleteSemaphore;

            err = vkAcquireNextImageKHR(device,
                                        m_ImGuiWindow->Swapchain,
                                        UINT64_MAX,
                                        image_acquired_semaphore,
                                        VK_NULL_HANDLE,
                                        &m_ImGuiWindow->FrameIndex);

            if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
                m_SwapChainValid = false;
                return;
            }

            ImGui_ImplVulkanH_Frame* fd = &m_ImGuiWindow->Frames[m_ImGuiWindow->FrameIndex];
            {
                VK_CHECK(vkWaitForFences(device, 1, &fd->Fence, VK_TRUE, UINT64_MAX));
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
                info.renderPass               = m_ImGuiWindow->RenderPass;
                info.framebuffer              = fd->Framebuffer;
                info.renderArea.extent.width  = m_ImGuiWindow->Width;
                info.renderArea.extent.height = m_ImGuiWindow->Height;
                info.clearValueCount          = 1;
                info.pClearValues             = &m_ImGuiWindow->ClearValue;
                vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
            }

            ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

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

        // Present Frame
        {
            VkSemaphore      render_complete_semaphore = m_ImGuiWindow->FrameSemaphores[m_ImGuiWindow->SemaphoreIndex].RenderCompleteSemaphore;
            VkPresentInfoKHR info                      = {};
            info.sType                                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            info.waitSemaphoreCount                    = 1;
            info.pWaitSemaphores                       = &render_complete_semaphore;
            info.swapchainCount                        = 1;
            info.pSwapchains                           = &m_ImGuiWindow->Swapchain;
            info.pImageIndices                         = &m_ImGuiWindow->FrameIndex;
            VkResult err                               = vkQueuePresentKHR(m_Renderer->GetGraphicsQueue()->Handle(), &info);
            if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
                m_SwapChainValid = false;
                return;
            }
            m_ImGuiWindow->SemaphoreIndex = (m_ImGuiWindow->SemaphoreIndex + 1) % m_ImGuiWindow->ImageCount;
        }
    }

    void Window::BeginFrame() {
        RefreshSwapChain();

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();
    }
    void Window::EndFrame() {
        RenderFrame();
    }

}  // namespace Vulkan