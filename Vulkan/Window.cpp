#include "Window.h"

#include <GLFW/glfw3.h>
#include <IMGUI/backends/imgui_impl_glfw.h>
#include <IMGUI/backends/imgui_impl_vulkan.h>

#include "Vulkan/Device.h"
#include "Vulkan/Helpers.h"
#include "Vulkan/Queue.h"
#include "Vulkan/Renderer.h"

static void check_vk_result(VkResult err) {
    if (err == 0) return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0) abort();
}

Ref<Window> Window::Create(Ref<Vulkan::Renderer> renderer) {
    return Ref<Window>(new Window(renderer));
}

Window::Window(Ref<Vulkan::Renderer> renderer) {
    uint32_t minImageCount = 2;
    DE_ASSERT(renderer, "Bad renderer");
    m_Renderer = renderer;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_WindowHandle = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    DE_ASSERT(m_WindowHandle, "Failed to create Vulkan Window");

    VK_CHECK(glfwCreateWindowSurface(m_Renderer->GetInstanceHandle(), m_WindowHandle, nullptr, &m_Surface));

    {
        int w, h;
        glfwGetFramebufferSize(m_WindowHandle, &w, &h);
        m_ImGuiWindow          = CreateRef<ImGui_ImplVulkanH_Window>();
        m_ImGuiWindow->Surface = m_Surface;

        auto supportDetails = Vulkan::Helpers::GetSwapChainSupportDetails(m_Renderer->GetPhysicalDevice(), m_Surface);

        const VkFormat requestSurfaceImageFormat[] = {
            VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        m_ImGuiWindow->SurfaceFormat                   = ImGui_ImplVulkanH_SelectSurfaceFormat(m_Renderer->GetPhysicalDevice(),
                                                                             m_Surface,
                                                                             requestSurfaceImageFormat,
                                                                             (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat),
                                                                             requestSurfaceColorSpace);

        VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
        m_ImGuiWindow->PresentMode =
            ImGui_ImplVulkanH_SelectPresentMode(m_Renderer->GetPhysicalDevice(), m_Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));

        // Create SwapChain, RenderPass, Framebuffer, etc.
        ImGui_ImplVulkanH_CreateOrResizeWindow(m_Renderer->GetInstanceHandle(),
                                               m_Renderer->GetPhysicalDevice(),
                                               m_Renderer->GetLogicDevice(),
                                               m_ImGuiWindow.get(),
                                               *m_Renderer->GetDevice()->GetFamilyIndex(Vulkan::QueueFamily::Graphics),
                                               nullptr,
                                               w,
                                               h,
                                               minImageCount);
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
        pool_info.maxSets                    = 1;
        pool_info.poolSizeCount              = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes                 = pool_sizes;
        VK_CHECK(vkCreateDescriptorPool(m_Renderer->GetLogicDevice(), &pool_info, nullptr, &m_DescriptorPool));
    }
    {
        ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance                  = m_Renderer->GetInstanceHandle();
        init_info.PhysicalDevice            = m_Renderer->GetPhysicalDevice();
        init_info.Device                    = m_Renderer->GetLogicDevice();
        init_info.QueueFamily               = *m_Renderer->GetDevice()->GetFamilyIndex(Vulkan::QueueFamily::Graphics);
        init_info.Queue                     = m_Renderer->GetDevice()->GetQueue(Vulkan::QueueFamily::Graphics, 0)->Handle();
        init_info.PipelineCache             = VK_NULL_HANDLE;
        init_info.DescriptorPool            = m_DescriptorPool;
        init_info.Subpass                   = 0;
        init_info.MinImageCount             = minImageCount;
        init_info.ImageCount                = m_ImGuiWindow->ImageCount;
        init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator                 = nullptr;
        init_info.CheckVkResultFn           = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, m_ImGuiWindow->RenderPass);
    }
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_WindowHandle);
}

GLFWwindow* Window::Handle() {
    return m_WindowHandle;
}
ImGui_ImplVulkanH_Window* Window::ImGuiWindow() {
    return m_ImGuiWindow.get();
}

Window::~Window() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    vkDestroyDescriptorPool(m_Renderer->GetLogicDevice(), m_DescriptorPool, nullptr);
    ImGui::DestroyContext();
    ImGui_ImplVulkanH_DestroyWindow(m_Renderer->GetInstanceHandle(), m_Renderer->GetLogicDevice(), m_ImGuiWindow.get(), nullptr);
    m_ImGuiWindow = nullptr;
    glfwDestroyWindow(m_WindowHandle);
}