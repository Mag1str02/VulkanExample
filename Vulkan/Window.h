#pragma once

#include "Base.h"
#include "Instance.h"

class GLFWwindow;

class Window {
public:
    static Ref<Window> Create();
    ~Window();

    bool         ShouldClose() const;
    void         SetInstance(Ref<Vulkan::Instance> instnace);
    GLFWwindow*  Handle();
    VkSurfaceKHR Surface() const;

    NO_COPY_CONSTRUCTORS(Window);
    NO_MOVE_CONSTRUCTORS(Window);

private:
    Window();

    Ref<Vulkan::Instance> m_Instance      = nullptr;
    GLFWwindow*           m_WindowHandle  = nullptr;
    VkSurfaceKHR          m_SurfaceHandle = VK_NULL_HANDLE;
};