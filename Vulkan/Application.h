#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Vulkan/CommandBuffer.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/Device.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Queue.h"
#include "Vulkan/Renderer.h"
#include "Vulkan/SwapChain.h"


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

protected:
    Ref<Window>           m_Window;
    Ref<Vulkan::Renderer> m_Renderer;
};
