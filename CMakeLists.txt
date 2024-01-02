cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
set(SPIRV_COMPILER "glslc")

project(VulkanExample)

include(CMake/Shaders.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Sandbox)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Sandbox)

add_subdirectory(CMake)
add_subdirectory(Examples)
add_subdirectory(Vulkan)
add_subdirectory(Dependencies)

