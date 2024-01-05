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

GLFWwindow* Window::Handle() {
    return m_WindowHandle;
}

Window::~Window() {
    glfwDestroyWindow(m_WindowHandle);
}