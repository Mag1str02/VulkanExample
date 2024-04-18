add_library(VulkanHelpers
    Application/Application.cpp
    Application/Window.cpp

    Utils/Assert.cpp

    
    Vulkan/Executor/Task.cpp
    Vulkan/Executor/Task.cpp

    Vulkan/Objects/SwapChain.cpp
    Vulkan/Objects/Buffer.cpp
    Vulkan/Objects/CommandBuffer.cpp
    Vulkan/Objects/CommandPool.cpp
    Vulkan/Objects/Debugger.cpp
    Vulkan/Objects/Device.cpp
    Vulkan/Objects/Fence.cpp
    Vulkan/Objects/Image.cpp
    Vulkan/Objects/ImageSampler.cpp
    Vulkan/Objects/ImageView.cpp
    Vulkan/Objects/Instance.cpp
    Vulkan/Objects/Queue.cpp

    Vulkan/Renderer/Config.cpp
    Vulkan/Renderer/ImGuiWindow.cpp
    Vulkan/Renderer/Renderer.cpp
    Vulkan/Renderer/Window.cpp

    Vulkan/Helpers.cpp
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