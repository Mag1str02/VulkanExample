#pragma once

#include "Base.h"
#include "Instance.h"

class GLFWwindow;

class Window {
public:
    static Ref<Window> Create();
    ~Window();

    bool        ShouldClose() const;
    GLFWwindow* Handle();

    NO_COPY_CONSTRUCTORS(Window);
    NO_MOVE_CONSTRUCTORS(Window);

private:
    Window();

    GLFWwindow* m_WindowHandle = nullptr;
};