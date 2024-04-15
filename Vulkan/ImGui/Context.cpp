#include "Context.h"

#include <backends/imgui_impl_vulkan.h>

namespace Vulkan::ImGui {
    Context::Context() {
        {
            ::IMGUI_CHECKVERSION();
            ::ImGui::CreateContext();
            ImGuiIO& io = ::ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
            // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows
        }
    }

    Context::~Context() {
        ::ImGui::DestroyContext();
    }

    void Context::BeginFrame() {
        ::ImGui::NewFrame();
    }

    ImDrawData* Context::Render() {
        ::ImGui::EndFrame();
        ::ImGui::Render();
        return ::ImGui::GetDrawData();
    }
}  // namespace Vulkan::ImGui