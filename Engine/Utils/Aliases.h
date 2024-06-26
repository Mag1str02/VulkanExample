#pragma once

#include <chrono>
#include <memory>

#include <glm/glm.hpp>

namespace Engine {

    using namespace std::chrono_literals;
    using UVec2 = glm::u32vec2;
    using Vec4  = glm::vec4;

    template <typename T>
    using Scope = std::unique_ptr<T>;
    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T>
    using CRef = std::shared_ptr<const T>;
    template <typename T>
    using WeakRef = std::weak_ptr<T>;
    template <typename T>
    using RefCounted = std::enable_shared_from_this<T>;

    template <typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}  // namespace Engine