add_executable(Example main.cpp)
target_link_libraries(Example 
    PUBLIC VulkanHelpers
)

# target_shaders(
#     TARGET
#     Example
#     SHADERS
#         Triangle.vert
#         Triangle.frag
# )