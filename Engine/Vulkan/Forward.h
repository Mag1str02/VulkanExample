#pragma once

namespace Engine {

    class Window;

    namespace Vulkan {

        namespace Interface {
            class Image;
            class SwapChain;
        }  // namespace Interface
        namespace Managed {
            class Image;
        }
        namespace Concrete {
            class Image;
            class SwapChain;
        }  // namespace Concrete
        namespace Sync {
            struct Access;
            struct State;
            struct ImageState;
        }  // namespace Sync

        using IImage = Interface::Image;
        using ISwapChain = Interface::SwapChain;

        class Instance;
        class Debugger;
        class Device;
        class Queue;
        class CommandPool;
        class CommandBuffer;
        class Buffer;
        class ImageSampler;
        class ImageView;

        class Config;
        class Renderer;
        class Window;

    }  // namespace Vulkan

}  // namespace Engine