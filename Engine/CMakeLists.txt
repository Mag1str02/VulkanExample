add_library(VulkanHelpers
    Application/Application.cpp
    Application/Window.cpp

    Utils/Assert.cpp

    Vulkan/Concrete/Fence.cpp
    Vulkan/Concrete/CommandBuffer.cpp
    Vulkan/Concrete/Image.cpp
    Vulkan/Concrete/SmartCommandBuffer.cpp

    Vulkan/Interface/Fence.cpp
    Vulkan/Interface/Image.cpp

    Vulkan/Managed/Fence.cpp
    Vulkan/Managed/CommandBuffer.cpp
    Vulkan/Managed/Image.cpp

    Vulkan/RenderGraph/PassNode.cpp
    Vulkan/RenderGraph/ResourceNode.cpp
    Vulkan/RenderGraph/RenderGraph.cpp
    Vulkan/RenderGraph/TaskBuilder.cpp
    
    Vulkan/RenderGraph/FrameGraph/FrameGraph.cpp
    Vulkan/RenderGraph/FrameGraph/SwapChainAquirePassNode.cpp
    Vulkan/RenderGraph/FrameGraph/SwapChainPresentPassNode.cpp
    Vulkan/RenderGraph/FrameGraph/SwapChainImageNode.cpp
    Vulkan/RenderGraph/FrameGraph/SwapChainNodesState.cpp

    Vulkan/Synchronization/AccessScope.cpp
    Vulkan/Synchronization/ImageTracker.cpp
    Vulkan/Synchronization/Tracker.cpp
    
    Vulkan/Renderer/Config.cpp
    Vulkan/Renderer/Debugger.cpp
    Vulkan/Renderer/Device.cpp
    Vulkan/Renderer/Instance.cpp
    Vulkan/Renderer/GraphicsQueue.cpp
    Vulkan/Renderer/PresentationQueue.cpp
    Vulkan/Renderer/Queue.cpp
    Vulkan/Renderer/Renderer.cpp
    Vulkan/Renderer/SemaphorePool.cpp
    Vulkan/Renderer/Task.cpp
    Vulkan/Renderer/Executor.cpp

    Vulkan/Window/SwapChain.cpp
    Vulkan/Window/Surface.cpp
    Vulkan/Window/Window.cpp

    Vulkan/Buffer.cpp
    Vulkan/CommandPool.cpp
    Vulkan/ImageSampler.cpp
    Vulkan/ImageView.cpp

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