#include "Application.h"

#include "Engine/Utils/Assert.h"

#include <tracy/Tracy.hpp>

#include <GLFW/glfw3.h>

namespace Engine {
    Application::Application() {
        glfwInitHint(GLFW_WIN32_MESSAGES_IN_FIBER, GLFW_TRUE);
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
            FrameMarkStart("Frame");
            Loop();
            FrameMarkEnd("Frame");
        }
        OnShutDown();
    }

    void Application::Loop() {
        ZoneScopedN("Loop");
        m_Window->BeginFrame();
        OnLoop();
        m_Window->EndFrame();
    }
}  // namespace Engine