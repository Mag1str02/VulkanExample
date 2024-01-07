#pragma once

#include "Vulkan/Common.h"
#include "Vulkan/Instance.h"

class ImGui_ImplVulkanH_Window;
class GLFWwindow;

class Window {
public:
    static Ref<Window> Create(Ref<Vulkan::Renderer> renderer);
    ~Window();

    bool        ShouldClose() const;
    GLFWwindow* Handle();
    ImGui_ImplVulkanH_Window* ImGuiWindow();

    NO_COPY_CONSTRUCTORS(Window);
    NO_MOVE_CONSTRUCTORS(Window);

private:
    Window(Ref<Vulkan::Renderer> renderer);

private:
    VkDescriptorPool              m_DescriptorPool = VK_NULL_HANDLE;
    VkSurfaceKHR                  m_Surface        = VK_NULL_HANDLE;
    GLFWwindow*                   m_WindowHandle   = nullptr;
    Ref<Vulkan::Renderer>         m_Renderer       = nullptr;
    Ref<ImGui_ImplVulkanH_Window> m_ImGuiWindow    = nullptr;
};