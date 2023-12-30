#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

class Application {
public:
    Application();
    ~Application();
    void Run();

protected:
    virtual void OnStartUp() {}
    virtual void OnLoop() {}
    virtual void OnShutDown() {}

private:
    void Loop();

    void InitGLFW();
    void TerminateGLFW();

    void InitVulkan();
    void TerminateVulkan();

    GLFWwindow* m_Window;
    VkInstance m_VkInstance;
};
