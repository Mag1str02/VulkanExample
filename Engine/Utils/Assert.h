#pragma once

#include <string>

namespace Engine {
    void FailAssert(const char* expr_str, const char* file, int line, const std::string& msg);

#define DE_ASSERT(Expr, msg)                                      \
    do {                                                          \
        if (!(Expr)) {                                            \
            ::Engine::FailAssert(#Expr, __FILE__, __LINE__, msg); \
        }                                                         \
    } while (false)
}  // namespace Engine