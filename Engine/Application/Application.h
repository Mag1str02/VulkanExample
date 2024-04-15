#pragma once

#include "Engine/Vulkan/CommandBuffer.h"
#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Device.h"
#include "Engine/Vulkan/Queue.h"
#include "Engine/Vulkan/Renderer.h"

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
        Ref<Vulkan::Window>   m_Window;
        Ref<Vulkan::Renderer> m_Renderer;
    };

}  // namespace Engine
