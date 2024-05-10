#include "Window.h"

#include "Surface.h"

#include "Engine/Vulkan/Renderer/Instance.h"
#include "Engine/Vulkan/Renderer/Renderer.h"

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
        VkSurfaceKHR handle;
        VK_CHECK(glfwCreateWindowSurface(m_Renderer->GetInstance()->Handle(), m_WindowHandle, nullptr, &handle));
        auto surface = Surface::Create(handle, m_Renderer->GetDevice());
        m_FrameGraph.Construct(surface);
    }

    Window::~Window() {}

    void Window::BeginFrame() {
        PROFILER_SCOPE("Engine::Vulkan::Window::BeginFrame");
        Engine::Window::BeginFrame();
    }
    void Window::EndFrame() {
        PROFILER_SCOPE("Engine::Vulkan::Window::EndFrame");
        Engine::Window::EndFrame();
    }

    RenderGraph::RenderGraph* Window::GetRenderGraph() const {
        return m_FrameGraph->GetInternalGraph();
    }

}  // namespace Engine::Vulkan