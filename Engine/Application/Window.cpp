#include "Window.h"

#include <GLFW/glfw3.h>

namespace Engine {

    Window::Window() {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_WindowHandle = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
        DE_ASSERT(m_WindowHandle, "Failed to create Vulkan Window");
    }

    bool Window::ShouldClose() const {
        return glfwWindowShouldClose(m_WindowHandle);
    }

    GLFWwindow* Window::Handle() {
        return m_WindowHandle;
    }

    std::pair<uint32_t, uint32_t> Window::GetSize() const {
        int w, h;
        glfwGetFramebufferSize(m_WindowHandle, &w, &h);
        return {w, h};
    }

    Window::~Window() {
        glfwDestroyWindow(m_WindowHandle);
    }

    void Window::BeginFrame() {}
    void Window::EndFrame() {}

}  // namespace Engine
