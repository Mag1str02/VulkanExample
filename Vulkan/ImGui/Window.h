#pragma once

#include "Vulkan/Common.h"
#include "Vulkan/ImGui/Context.h"
#include "Vulkan/Object.h"

namespace Vulkan::ImGui {
    class Window : public Object {
    public:
        Window(Ref<Renderer> renderer, VkSurfaceKHR surface) : m_Renderer(renderer), m_Surface(surface) {}
        ~Window();

    private:
        ImGui_ImplVulkanH_Window m_Window;
        Context                  m_Context;

        Ref<Renderer>    m_Renderer       = nullptr;
        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
        VkSurfaceKHR     m_Surface        = VK_NULL_HANDLE;
    };
}  // namespace Vulkan::ImGui