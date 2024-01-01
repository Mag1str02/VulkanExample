#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>

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

    GLFWwindow* m_Window = nullptr;

    VkInstance               m_VkInstance     = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessanger = VK_NULL_HANDLE;
    VkPhysicalDevice         m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice                 m_LogicDevice    = VK_NULL_HANDLE;
    VkQueue                  m_PresentQueue   = VK_NULL_HANDLE;
    VkQueue                  m_GraphicsQueue  = VK_NULL_HANDLE;
    VkSurfaceKHR             m_Surface        = VK_NULL_HANDLE;
    VkSwapchainKHR           m_SwapChain      = VK_NULL_HANDLE;

    std::vector<VkImage>     m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkFormat                 m_SwapChainImageFormat;
    VkExtent2D               m_SwapChainExtent;
};
