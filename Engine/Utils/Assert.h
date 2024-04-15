#pragma once

#include <string>

void FailAssert(const char* expr_str, const char* file, int line, const std::string& msg);

#define DE_ASSERT(Expr, msg)                                            \
    do {                                                                \
        if (!(Expr)) {                                                  \
            FailAssert(#Expr, __FILE__, __LINE__, msg); \
        }                                                               \
    } while (false)
