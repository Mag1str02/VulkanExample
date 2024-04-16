#pragma once

#include "Engine/Utils/Base.h"

class GLFWwindow;

namespace Engine {
    class Window {
    public:
        Window();
        virtual ~Window();

        bool        ShouldClose() const;
        UVec2       GetSize() const;
        GLFWwindow* Handle();

        virtual void BeginFrame();
        virtual void EndFrame();

        NO_COPY_CONSTRUCTORS(Window);
        NO_MOVE_CONSTRUCTORS(Window);

    private:
        GLFWwindow* m_WindowHandle = nullptr;
    };
}  // namespace Engine