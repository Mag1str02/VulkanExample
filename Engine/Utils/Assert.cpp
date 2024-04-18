#include "Assert.h"

#include <iostream>
#include <sstream>

namespace Engine {

    void FailAssert(const char* expr_str, const char* file, int line, const std::string& msg) {
        std::stringstream ss;

        ss << "\n"
           << "Assert failed:\t" << msg << "\n"
           << "Expected:\t" << expr_str << "\n"
           << "Source:\t\t" << file << ", line " << line << "\n";
        std::cerr << ss.str() << std::endl;
        exit(1);
    }

}  // namespace Engine
