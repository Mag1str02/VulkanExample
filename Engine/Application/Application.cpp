#include "Application.h"

#include "Engine/Utils/Assert.h"

#include <GLFW/glfw3.h>

namespace Engine {
    Application::Application() {
        glfwInit();
        volkInitialize();

        m_Renderer.Construct();
        m_Window.Construct(m_Renderer.Get());

        OnStartUp();
    }
    Application::~Application() {
        OnShutDown();

        m_Window.Destruct();
        m_Renderer.Destruct();

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