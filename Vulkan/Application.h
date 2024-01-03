#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>

#include "Renderer.h"

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

protected:
    Scope<Vulkan::Renderer> m_Renderer;
    Ref<Window>             m_Window;
    VkSwapchainKHR          m_SwapChain     = VK_NULL_HANDLE;

    uint32_t m_GraphicsQueueFamilyIndex;

    std::vector<VkImage>     m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkFormat                 m_SwapChainImageFormat;
    VkExtent2D               m_SwapChainExtent;
};
