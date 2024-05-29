add_library(VulkanHelpers
    Application/Application.cpp
    Application/Window.cpp

    Utils/Assert.cpp

    Vulkan/Managed/Fence.cpp
    Vulkan/Managed/RawCommandBuffer.cpp
    Vulkan/Managed/Image.cpp
    Vulkan/Managed/CommandPool.cpp

    Vulkan/RenderGraph/PassNode.cpp
    Vulkan/RenderGraph/RenderGraph.cpp
    Vulkan/RenderGraph/TaskBuilder.cpp
    Vulkan/RenderGraph/ResourceNode.cpp
    Vulkan/RenderGraph/ProxyResourceNode.cpp
    Vulkan/RenderGraph/ExternalDependencyManager.cpp
    Vulkan/RenderGraph/CommandBufferPassNode.cpp
    
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
    Vulkan/Renderer/Executor.cpp

    Vulkan/Window/SwapChain.cpp
    Vulkan/Window/Surface.cpp
    Vulkan/Window/Window.cpp

    Vulkan/Window/FrameGraph/FrameGraph.cpp
    Vulkan/Window/FrameGraph/SwapChainAquirePassNode.cpp
    Vulkan/Window/FrameGraph/SwapChainPresentPassNode.cpp
    Vulkan/Window/FrameGraph/SwapChainImageNode.cpp
    Vulkan/Window/FrameGraph/SwapChainNodesState.cpp


    Vulkan/Fence.cpp
    Vulkan/FencePool.cpp
    Vulkan/CommandBuffer.cpp
    Vulkan/Image.cpp
    Vulkan/RawCommandBuffer.cpp
    Vulkan/Buffer.cpp
    Vulkan/CommandPool.cpp
    Vulkan/ImageSampler.cpp
    Vulkan/BinarySemaphore.cpp
    Vulkan/BinarySemaphorePool.cpp
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