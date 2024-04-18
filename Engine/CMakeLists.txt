add_library(VulkanHelpers
    Application/Application.cpp
    Application/Window.cpp

    Utils/Assert.cpp
    Utils/Handles.cpp

    Vulkan/Buffer.cpp
    Vulkan/CommandBuffer.cpp
    Vulkan/CommandPool.cpp
    Vulkan/Config.cpp
    Vulkan/Debugger.cpp
    Vulkan/Device.cpp
    Vulkan/Helpers.cpp
    Vulkan/Image.cpp
    Vulkan/ImageSampler.cpp
    Vulkan/ImageView.cpp
    Vulkan/ImGuiWindow.cpp
    Vulkan/Instance.cpp
    Vulkan/Renderer.cpp
    Vulkan/SwapChain.cpp
    Vulkan/Window.cpp
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