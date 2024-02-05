set(VOLK_PULL_IN_VULKAN OFF)

add_subdirectory(VOLK)

target_include_directories(volk
    PUBLIC VULKAN_HEADERS/include
)
target_compile_definitions(volk PUBLIC VK_NO_PROTOTYPES)