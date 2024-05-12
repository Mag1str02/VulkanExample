#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::RenderGraph::Interface {

    class DynamicType {
    public:
        virtual ~DynamicType() = default;

        template <typename T>
        bool Is() const {
            return dynamic_cast<const T*>(this) != nullptr;
        }
        template <typename T>
        T* As() {
            T* ptr = dynamic_cast<T*>(this);
            DE_ASSERT(ptr != nullptr, "Bad type");
            return ptr;
        }
        template <typename T>
        const T* As() const {
            const T* ptr = dynamic_cast<T*>(this);
            DE_ASSERT(ptr != nullptr, "Bad type");
            return ptr;
        }
    };

}  // namespace Engine::Vulkan::RenderGraph::Interface