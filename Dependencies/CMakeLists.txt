add_subdirectory(GLFW)
add_subdirectory(GLM)

find_package(Vulkan REQUIRED)

add_library(Dependencies dummy.cpp)
target_include_directories(Dependencies
    PUBLIC GLFW/include
    PUBLIC GLM
    PUBLIC ${Vulkan_INCLUDE_DIRS}
)
target_link_libraries(Dependencies
    PUBLIC glfw
    PUBLIC glm
    PUBLIC ${Vulkan_LIBRARIES}
)
