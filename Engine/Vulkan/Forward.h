#pragma once

namespace Engine {

    class Window;

    namespace Vulkan {

        namespace Interface {
            class Image;
        }
        namespace Managed {
            class Image;
        }
        namespace Concrete {
            class Image;
        }
        namespace Sync {
            struct Access;
            struct State;
            struct ImageState;
        }  // namespace Sync

        using IImage = Interface::Image;

        class ISwapChain;

        class Instance;
        class Debugger;
        class Device;
        class Queue;
        class CommandPool;
        class CommandBuffer;
        class Buffer;
        class ImageSampler;
        class ImageView;
        class SwapChain;

        class Config;
        class Renderer;
        class Window;

    }  // namespace Vulkan

}  // namespace Engine