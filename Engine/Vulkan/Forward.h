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
            namespace Interface {
                enum class DependencyType;
                class DynamicType;
                class Entry;
                class Node;
                class Pass;
                class PassNode;
                class PassEntry;
                class PassCluster;
                class StaticResourceNode;
                class ImageResourceNode;
            }  // namespace Interface

            using Interface::DependencyType;
            using Interface::DynamicType;
            using Interface::ImageResourceNode;
            using Interface::StaticResourceNode;

            using IEntry       = Interface::Entry;
            using INode        = Interface::Node;
            using IPass        = Interface::Pass;
            using IPassNode    = Interface::PassNode;
            using IPassEntry   = Interface::PassEntry;
            using IPassCluster = Interface::PassCluster;

            class ResourceNode;
            class PassNode;
            class RenderGraph;
            class TaskBuilder;

            class SwapChainNodesState;
            class SwapChainAquirePassNode;
            class SwapChainPresentPassNode;
            class SwapChainImageNode;

        }  // namespace RenderGraph

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
        class Executor;
        class Semaphore;
        class SemaphorePool;

        using IImage = Interface::Image;
        using ITask  = Task;

    }  // namespace Vulkan

}  // namespace Engine