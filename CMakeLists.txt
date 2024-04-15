cmake_minimum_required(VERSION 3.0.0)

project(VulkanExample)

set(SPIRV_COMPILER "glslc")
set(SANITIZERS_ENABLED OFF)

include(CMake/Compiler.cmake)
include(CMake/Shaders.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Sandbox)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Sandbox)

add_subdirectory(CMake)
add_subdirectory(Engine)
add_subdirectory(Examples)
add_subdirectory(Dependencies)

