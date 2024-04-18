#include "Window.h"

#include "Device.h"
#include "Helpers.h"
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
        VK_CHECK(glfwCreateWindowSurface(m_Renderer->GetInstanceHandle(), m_WindowHandle, nullptr, &m_Surface));
        auto size   = GetSize();
        // m_SwapChain = CreateRef<SwapChain>(m_Surface, m_Renderer, VkExtent2D{.width = size.x, .height = size.y});
    }

    Window::~Window() {
        // m_SwapChain = nullptr;
        vkDestroySurfaceKHR(m_Renderer->GetInstanceHandle(), m_Surface, nullptr);
    }

    void Window::BeginFrame() {
        Engine::Window::BeginFrame();
    }
    void Window::EndFrame() {
        Engine::Window::EndFrame();
    }

    VkSurfaceKHR Window::GetSurface() {
        return m_Surface;
    }

}  // namespace Engine::Vulkan