#include "Window.h"

#include <GLFW/glfw3.h>

Ref<Window> Window::Create() {
    return Ref<Window>(new Window());
}

Window::Window() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_WindowHandle = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    DE_ASSERT(m_WindowHandle, "Failed to create Vulkan Window");
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_WindowHandle);
}

void Window::SetInstance(Ref<Vulkan::Instance> isntance) {
    if (m_Instance) {
        vkDestroySurfaceKHR(m_Instance->Handle(), m_SurfaceHandle, nullptr);
        m_Instance      = nullptr;
        m_SurfaceHandle = VK_NULL_HANDLE;
    }
    if (isntance) {
        m_Instance = isntance;
        auto res   = glfwCreateWindowSurface(m_Instance->Handle(), m_WindowHandle, nullptr, &m_SurfaceHandle);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create window surface");
    }
}

GLFWwindow* Window::Handle() {
    return m_WindowHandle;
}

VkSurfaceKHR Window::Surface() const {
    DE_ASSERT(m_Instance, "Cannot get window surface because window has no vulkan instance");
    return m_SurfaceHandle;
}

Window::~Window() {
    SetInstance(nullptr);
    glfwDestroyWindow(m_WindowHandle);
}