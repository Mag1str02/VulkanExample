#pragma once

#include "Common.h"
#include "Object.h"

#include "Engine/Application/Window.h"

namespace Engine::Vulkan {

    class Window : public Engine::Window {
    public:
        Window(Ref<Renderer> renderer);
        ~Window();

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

    private:
        Ref<Renderer>  m_Renderer  = nullptr;
        Ref<SwapChain> m_SwapChain = nullptr;
    };
}  // namespace Engine::Vulkan