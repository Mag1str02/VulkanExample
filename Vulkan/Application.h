#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Vulkan/Device.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Queue.h"
#include "Vulkan/Renderer.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/CommandPool.h"

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
    Ref<Window>            m_Window;
    Ref<Vulkan::Renderer>  m_Renderer;
    Ref<Vulkan::SwapChain> m_SwapChain;

    std::vector<VkImageView> m_SwapChainImageViews;
};
