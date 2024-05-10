#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::RenderGraph {

    class Entry : NonCopyMoveable {
    public:
        virtual ~Entry() = default;

        virtual std::optional<std::string> Validate() const = 0;
        virtual const std::string&         GetName() const  = 0;

        template <typename T>
        bool Is() const {
            return dynamic_cast<T*>(this) != nullptr;
        }
        template <typename T>
        T* As() const {
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
}  // namespace Engine::Vulkan::RenderGraph