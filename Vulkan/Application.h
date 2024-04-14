#pragma once

// clang-format off
#include <volk.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "Vulkan/CommandBuffer.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/Device.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Queue.h"
#include "Vulkan/Renderer.h"

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
    Ref<Vulkan::Window>   m_Window;
    Ref<Vulkan::Renderer> m_Renderer;
};
