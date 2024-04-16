#include "Window.h"

#include "Device.h"
#include "Helpers.h"
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

    Window::Window(Ref<Renderer> renderer) : m_Renderer(renderer) {
        m_SwapChain = CreateRef<SwapChain>(this, m_Renderer->GetDevice());
    }

    Window::~Window() {}

    void Window::BeginFrame() {
        Engine::Window::BeginFrame();
    }
    void Window::EndFrame() {
        Engine::Window::EndFrame();
    }

}  // namespace Engine::Vulkan