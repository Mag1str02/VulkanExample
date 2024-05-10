#pragma once

namespace Engine {

    class Window;

    namespace Vulkan {

        namespace Interface {
            class Image;
        }  // namespace Interface
        namespace Managed {
            class Fence;
            class Image;
            class CommandBuffer;
        }  // namespace Managed
        namespace Concrete {
            class Fence;
            class Image;
            class CommandBuffer;
        }  // namespace Concrete
        namespace Synchronization {
            class AccessScope;
            class ImageTracker;
        }  // namespace Synchronization

        namespace RenderGraph {
            class Node;
            class ResourceNode;
            class IPassNode;
            class PassNode;
            class RenderGraph;

            class Pass;
            class Resource;
        }  // namespace RenderGraph

        using IImage = Interface::Image;

        class Instance;
        class Debugger;
        class Device;
        class Queue;
        class CommandPool;
        class Buffer;
        class ImageSampler;
        class ImageView;
        class Surface;
        class SwapChain;

        class Config;
        class Renderer;
        class Window;
        class Task;

    }  // namespace Vulkan

}  // namespace Engine