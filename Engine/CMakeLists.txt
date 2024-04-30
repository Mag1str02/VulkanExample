add_library(VulkanHelpers
    Application/Application.cpp
    Application/Window.cpp

    Utils/Assert.cpp

    Vulkan/Concrete/Fence.cpp
    Vulkan/Concrete/CommandBuffer.cpp
    Vulkan/Concrete/Image.cpp
    Vulkan/Concrete/ResizebleSwapChain.cpp
    Vulkan/Concrete/SmartCommandBuffer.cpp
    Vulkan/Concrete/SwapChain.cpp

    Vulkan/Interface/Fence.cpp
    Vulkan/Interface/Image.cpp
    Vulkan/Interface/Semaphore.cpp
    Vulkan/Interface/SwapChain.cpp

    Vulkan/Managed/Fence.cpp
    Vulkan/Managed/CommandBuffer.cpp
    Vulkan/Managed/Image.cpp

    Vulkan/Synchronization/State.cpp
    Vulkan/Synchronization/Tracker.cpp
    
    Vulkan/Buffer.cpp
    Vulkan/CommandPool.cpp
    Vulkan/Debugger.cpp
    Vulkan/Device.cpp
    Vulkan/ImageSampler.cpp
    Vulkan/ImageView.cpp
    Vulkan/Instance.cpp
    Vulkan/Queue.cpp
    Vulkan/Task.cpp

    Vulkan/Config.cpp
    Vulkan/ImGuiWindow.cpp
    Vulkan/Renderer.cpp
    Vulkan/Window.cpp

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