#pragma once

namespace Engine::Vulkan::RenderGraph::Interface {

    enum class DependencyType {
        ReadOnlyInput,
        ReadWriteInput,
        Output,
    };

}