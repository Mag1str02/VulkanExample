#pragma once

#include <expected>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

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
