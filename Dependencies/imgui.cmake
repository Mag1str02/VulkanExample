add_library(imgui
    IMGUI/imgui.cpp
    IMGUI/backends/imgui_impl_glfw.cpp
    IMGUI/backends/imgui_impl_vulkan.cpp
    IMGUI/imgui_demo.cpp
    IMGUI/imgui_tables.cpp
    IMGUI/imgui_widgets.cpp
    IMGUI/imgui_draw.cpp
    IMGUI/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui
    PUBLIC IMGUI
    PUBLIC IMGUI/backends
    # PUBLIC VULKAN_HEADERS/include
)

target_link_libraries(imgui 
    PUBLIC glfw
    PUBLIC volk
)

# target_compile_definitions(imgui PRIVATE VK_NO_PROTOTYPES)