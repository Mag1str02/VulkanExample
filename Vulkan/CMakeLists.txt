add_library(VulkanHelpers
    Utils/Assert.cpp

    Application.cpp
    CommandBuffer.cpp
    CommandPool.cpp
    Debugger.cpp
    Device.cpp
    Helpers.cpp
    Instance.cpp
    Pipeline.cpp
    Queue.cpp
    Renderer.cpp
    ShaderModule.cpp
    SwapChain.cpp
    Window.cpp
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