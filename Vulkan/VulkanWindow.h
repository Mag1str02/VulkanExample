#pragma once

#include "Vulkan/Window.h"


class ImGui_ImplVulkanH_Window;

namespace Vulkan {
    class Window : public ::Window {
    public:
        Window(Ref<Renderer> renderer);
        ~Window();

        virtual void BeginFrame() override;
        virtual void EndFrame() override;

        ImGui_ImplVulkanH_Window* GetImGuiWindow();

    private:
        void RefreshSwapChain();
        void RenderFrame();

    private:
        bool                          m_SwapChainValid = false;
        Ref<ImGui_ImplVulkanH_Window> m_ImGuiWindow    = nullptr;
        Ref<Renderer>                 m_Renderer       = nullptr;
        VkDescriptorPool              m_DescriptorPool = VK_NULL_HANDLE;
        VkSurfaceKHR                  m_Surface        = VK_NULL_HANDLE;
    };
}  // namespace Vulkan