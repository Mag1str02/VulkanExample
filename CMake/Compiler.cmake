set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler: Sanitizers are ${SANITIZERS_ENABLED}")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC -fno-omit-frame-pointer")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    message(STATUS "Compiler: Using libc++")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    if (${SANITIZERS_ENABLED})
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
    endif()
endif()