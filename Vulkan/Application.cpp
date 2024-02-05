#include "Application.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "Vulkan/Debugger.h"
#include "Vulkan/Helpers.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Utils/Assert.h"
#include "Vulkan/Window.h"

namespace {}  // namespace

Application::Application() {
    glfwInit();
    
    volkInitialize();

    m_Renderer = CreateRef<Vulkan::Renderer>();
    m_Window   = Window::Create(m_Renderer);

    OnStartUp();
}
Application::~Application() {
    OnShutDown();

    m_Window   = nullptr;
    m_Renderer = nullptr;


    volkFinalize();
    glfwTerminate();
}
void Application::Run() {
    std::cerr << "Running" << std::endl;
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
