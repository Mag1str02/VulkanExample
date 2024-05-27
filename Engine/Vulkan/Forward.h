#pragma once

#include "Engine/Utils/Base.h"

namespace Engine {

    class Window;

    namespace Vulkan {

        namespace Interface {
            class Image;
            class Fence;
            class BinarySemaphore;
            class RawCommandBuffer;
            class Task;
        }  // namespace Interface

        using IRawCommandBuffer = Interface::RawCommandBuffer;
        using IImage            = Interface::Image;
        using IFence            = Interface::Fence;
        using ITask             = Interface::Task;
        using IBinarySemaphore  = Interface::BinarySemaphore;

        namespace Managed {
            class Fence;
            class Image;
            class CommandBuffer;
        }  // namespace Managed

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
                class PassFactory;
                class PassNode;
                class PassCluster;

                class StaticResourceNode;
                class ImageResourceNode;
                class ProxyResourceNode;
                class ResourceNode;
                class RemovableDependency;
            }  // namespace Interface

            using Interface::DependencyType;
            using Interface::DynamicType;

            using IEntry = Interface::Entry;
            using INode  = Interface::Node;

            using IPass        = Interface::Pass;
            using IPassFactory = Interface::PassFactory;
            using IPassNode    = Interface::PassNode;
            using IPassCluster = Interface::PassCluster;

            using IRemoveDependency   = Interface::RemovableDependency;
            using IProxyResourceNode  = Interface::ProxyResourceNode;
            using IResourceNode       = Interface::ResourceNode;
            using IStaticResourceNode = Interface::StaticResourceNode;
            using IImageResourceNode  = Interface::ImageResourceNode;

            class ResourceNode;
            class PassNode;
            class RenderGraph;
            class TaskBuilder;
            class ProxyResourceNode;
            class ExternalDependencyManager;

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
        class CommandBuffer;
        class Buffer;
        class ImageSampler;
        class ImageView;
        class Surface;
        class SwapChain;
        class Fence;
        class Image;

        class Config;
        class Renderer;
        class Window;

        class Executor;
        class BinarySemaphorePool;

    }  // namespace Vulkan

}  // namespace Engine