#pragma once

#include "Engine/Application/Window.h"
#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Window : public Engine::Window {
    public:
        Window(Renderer* renderer);
        ~Window();

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        VkSurfaceKHR GetSurface();

    private:
        VkSurfaceKHR m_Surface;

        Renderer*          m_Renderer  = nullptr;
        Ref<SwapChain>     m_SwapChain = nullptr;
        Ref<CommandBuffer> m_CommandBuffer;
    };
}  // namespace Engine::Vulkan