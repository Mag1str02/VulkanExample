#include "Window.h"

#include "Renderer.h"

#include "CommandPool.h"
#include "Device.h"
#include "ImageView.h"
#include "Instance.h"
#include "Queue.h"

#include "Engine/Vulkan/Concrete/CommandBuffer.h"
#include "Engine/Vulkan/Concrete/ResizebleSwapChain.h"

#include <GLFW/glfw3.h>

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
        auto size   = GetSize();
        m_SwapChain = Concrete::ResizebleSwapChain::Create(this, m_Renderer->GetDevice());
    }

    Window::~Window() {
        m_SwapChain.reset();
        vkDestroySurfaceKHR(m_Renderer->GetInstance()->Handle(), m_Surface, nullptr);
    }

    void Window::BeginFrame() {
        Engine::Window::BeginFrame();
        m_Renderer->Submit(m_SwapChain->CreateAquireImageTask());
        m_Renderer->GetQueue()->WaitIdle();
    }
    void Window::EndFrame() {
        Engine::Window::EndFrame();
    }

    VkSurfaceKHR Window::GetSurface() {
        return m_Surface;
    }
    VkExtent2D Window::GetExtent() {
        int w, h;
        glfwGetFramebufferSize(m_WindowHandle, &w, &h);
        VkExtent2D res;
        res.height = h;
        res.height = w;
        return res;
    }
    Ref<IImage> Window::GetSwapChainImage() {
        return m_SwapChain->GetCurrentImage();
    }

}  // namespace Engine::Vulkan