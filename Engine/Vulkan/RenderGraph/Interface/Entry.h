#pragma once

#include "DynamicType.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class Entry : virtual public DynamicType, NonCopyMoveable {
    public:
        virtual ~Entry() = default;

        virtual std::optional<std::string> Validate() const = 0;
    };
}  // namespace Engine::Vulkan::RenderGraph::Interface