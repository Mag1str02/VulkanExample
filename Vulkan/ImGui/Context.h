#pragma once

#include "Vulkan/Common.h"

#include <backends/imgui_impl_vulkan.h>

namespace Vulkan::ImGui {
    class Context {
    public:
        Context();
        ~Context();

        void        BeginFrame();
        ImDrawData* Render();
    };
}  // namespace Vulkan::ImGui