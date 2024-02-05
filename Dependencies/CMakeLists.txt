
add_subdirectory(GLFW)
add_subdirectory(GLM)
add_subdirectory(TPL)

include(vulkan.cmake)
include(imgui.cmake)

add_library(Dependencies dummy.cpp)
target_include_directories(Dependencies
    PUBLIC GLFW/include
    PUBLIC GLM
    PUBLIC IMGUI
)
target_link_libraries(Dependencies
    PUBLIC glfw
    PUBLIC glm
    PUBLIC imgui
    PUBLIC volk
)
