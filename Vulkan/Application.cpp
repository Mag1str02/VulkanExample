#include "Application.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "Vulkan/Helpers.h"
#include "Vulkan/Debugger.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Utils/Assert.h"
#include "Vulkan/Window.h"

namespace {}  // namespace

Application::Application() {
    InitGLFW();
    InitVulkan();
    OnStartUp();
}
Application::~Application() {
    OnShutDown();
    TerminateVulkan();
    TerminateGLFW();
}
void Application::Run() {
    OnStartUp();
    while (!m_Window->ShouldClose()) {
        Loop();
    }
    OnShutDown();
}

void Application::Loop() {
    glfwPollEvents();
    OnLoop();
}

void Application::InitGLFW() {
    glfwInit();

    m_Window = Window::Create();
}
void Application::TerminateGLFW() {
    m_Window = nullptr;
    glfwTerminate();
}

void Application::InitVulkan() {
    m_Renderer  = CreateRef<Vulkan::Renderer>();
    m_SwapChain = m_Renderer->GetDevice()->CreateSwapChain(m_Window);
}

void Application::TerminateVulkan() {
    m_SwapChain = nullptr;
    m_Renderer  = nullptr;
}