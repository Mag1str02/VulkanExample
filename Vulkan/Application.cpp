#include "Application.h"

#include "Vulkan/Debugger.h"
#include "Vulkan/Helpers.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Utils/Assert.h"
#include "Vulkan/VulkanWindow.h"
#include "Vulkan/Window.h"

Application::Application() {
    glfwInit();
    volkInitialize();

    m_Renderer = CreateRef<Vulkan::Renderer>();
    m_Window   = CreateRef<Vulkan::Window>(m_Renderer);

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
    m_Window->BeginFrame();
    OnLoop();
    m_Window->EndFrame();
}
