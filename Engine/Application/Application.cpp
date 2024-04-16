#include "Application.h"

#include "Engine/Utils/Assert.h"
#include "Engine/Vulkan/Debugger.h"
#include "Engine/Vulkan/Helpers.h"
#include "Engine/Vulkan/Instance.h"
#include "Engine/Vulkan/Window.h"


#include <GLFW/glfw3.h>

namespace Engine {
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
        m_Window->BeginFrame();
        OnLoop();
        m_Window->EndFrame();
    }
}  // namespace Engine