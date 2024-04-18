#pragma once

#include "Engine/Vulkan/CommandBuffer.h"
#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Device.h"
#include "Engine/Vulkan/Renderer.h"
#include "Engine/Vulkan/Window.h"

namespace Engine {

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
        ManualLifetime<Vulkan::Renderer> m_Renderer;
        ManualLifetime<Vulkan::Window>   m_Window;
    };

}  // namespace Engine
