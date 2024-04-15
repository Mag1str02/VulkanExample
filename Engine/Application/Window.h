#pragma once

#include "Engine/Vulkan/Common.h"
#include "Engine/Vulkan/Instance.h"

class GLFWwindow;

namespace Engine {
    class Window {
    public:
        Window();
        virtual ~Window();

        bool                          ShouldClose() const;
        std::pair<uint32_t, uint32_t> GetSize() const;
        GLFWwindow*                   Handle();

        virtual void BeginFrame();
        virtual void EndFrame();

        NO_COPY_CONSTRUCTORS(Window);
        NO_MOVE_CONSTRUCTORS(Window);

    private:
        GLFWwindow* m_WindowHandle = nullptr;
    };
}  // namespace Engine