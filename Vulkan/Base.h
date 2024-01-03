#pragma once

#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Utils/Singleton.h"
#include "Utils/Assert.h"

namespace fs = std::filesystem;

template <typename T> using Scope                            = std::unique_ptr<T>;
template <typename T> using Ref                              = std::shared_ptr<T>;
template <typename T> using CRef                             = std::shared_ptr<const T>;
template <typename T> using WeakRef                          = std::weak_ptr<T>;
template <typename T, typename E = std::string> using Result = std::expected<T, E>;

template <typename T, typename... Args> constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
template <typename T, typename... Args> constexpr Ref<T> CreateRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#define NO_COPY_CONSTRUCTORS(class_name)                     \
    class_name(const class_name& other)            = delete; \
    class_name& operator=(const class_name& other) = delete;

#define NO_MOVE_CONSTRUCTORS(class_name)                \
    class_name(class_name&& other)            = delete; \
    class_name& operator=(class_name&& other) = delete;

#define COPY_CONSTRUCTORS(class_name)    \
    class_name(const class_name& other); \
    class_name& operator=(const class_name& other);

#define MOVE_CONSTRUCTORS(class_name) \
    class_name(class_name&& other);   \
    class_name& operator=(class_name&& other);

#define DEFAULT_COPY_CONSTRUCTORS(class_name)                 \
    class_name(const class_name& other)            = default; \
    class_name& operator=(const class_name& other) = default;

#define DEFAULT_MOVE_CONSTRUCTORS(class_name)            \
    class_name(class_name&& other)            = default; \
    class_name& operator=(class_name&& other) = default;

#define DEFAULT_MOVE_CONSTRUCTORS_IMPL(class_name)          \
    class_name::class_name(class_name&& other) {            \
        Swap(other);                                        \
    }                                                       \
    class_name& class_name::operator=(class_name&& other) { \
        if (this == &other) {                               \
            return *this;                                   \
        }                                                   \
        Swap(other);                                        \
        return *this;                                       \
    }
