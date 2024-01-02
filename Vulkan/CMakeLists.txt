add_library(VulkanHelpers 
    Utils/Assert.cpp
    Application.cpp
    Config.cpp
    Debugger.cpp
    Instance.cpp
    Renderer.cpp
    ShaderModule.cpp
)

target_link_libraries(VulkanHelpers 
    PUBLIC Dependencies
)

target_include_directories(VulkanHelpers 
    PUBLIC ../.
)

target_compile_definitions(VulkanHelpers PRIVATE
    DE_VK_ENABLE_VALIDATION_LAYER=1
)