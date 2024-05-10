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

    private:
        Ref<Surface> m_Surface;

        Renderer* m_Renderer = nullptr;
    };
}  // namespace Engine::Vulkan