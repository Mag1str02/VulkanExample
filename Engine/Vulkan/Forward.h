#pragma once

namespace Engine {

    class Window;

    namespace Vulkan {

        namespace Interface {
            class Fence;
            class Semaphore;
            class Image;
            class SwapChain;
        }  // namespace Interface
        namespace Managed {
            class Fence;
            class Semaphore;
            class Image;
            class CommandBuffer;
        }  // namespace Managed
        namespace Concrete {
            class Fence;
            class Semaphore;
            class Image;
            class SwapChain;
            class CommandBuffer;
        }  // namespace Concrete
        namespace Synchronization {
            class AccessScope;
            class ImageState;
        }  // namespace Synchronization

        using IFence     = Interface::Fence;
        using ISemaphore = Interface::Semaphore;
        using IImage     = Interface::Image;
        using ISwapChain = Interface::SwapChain;

        class Instance;
        class Debugger;
        class Device;
        class Queue;
        class CommandPool;
        class Buffer;
        class ImageSampler;
        class ImageView;

        class Config;
        class Renderer;
        class Window;

    }  // namespace Vulkan

}  // namespace Engine