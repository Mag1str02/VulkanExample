#pragma once

#include "Common.h"
#include "Engine/Application/Window.h"

namespace Engine::Vulkan {

    class Window : public Engine::Window {
    public:
        Window(Renderer* renderer);
        ~Window();

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        VkSurfaceKHR GetSurface();
        VkExtent2D   GetExtent();
        Ref<IImage>  GetSwapChainImage();

    private:
        VkSurfaceKHR m_Surface;

        Renderer*       m_Renderer  = nullptr;
        Ref<ISwapChain> m_SwapChain = nullptr;
        Ref<IImage>     m_SwapChainImage;
    };
}  // namespace Engine::Vulkan