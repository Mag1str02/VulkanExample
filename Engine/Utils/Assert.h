#pragma once

#include <format>
#include <string>
#include <utility>

namespace Engine {
    void FailAssert(const char* expr_str, const char* file, int line, const std::string& msg);

#define DE_ASSERT(Expr, ...)                                                           \
    do {                                                                               \
        if (!(Expr)) {                                                                 \
            ::Engine::FailAssert(#Expr, __FILE__, __LINE__, std::format(__VA_ARGS__)); \
        }                                                                              \
    } while (false)
#define DE_CHECK(Expr)                                                                    \
    do {                                                                                  \
        if (!(Expr)) {                                                                    \
            ::Engine::FailAssert(#Expr, __FILE__, __LINE__, std::format("Check failed")); \
        }                                                                                 \
    } while (false)
#define DE_ASSERT_FAIL(...)                                                                   \
    do {                                                                                      \
        ::Engine::FailAssert("DE_ASSERT_FAIL", __FILE__, __LINE__, std::format(__VA_ARGS__)); \
        std::unreachable();                                                                   \
    } while (false);
#define DE_UNREACHABLE()                                                \
    do {                                                                \
        ::Engine::FailAssert("DE_UNREACHABLE", __FILE__, __LINE__, ""); \
        std::unreachable();                                             \
    } while (false);
}  // namespace Engine