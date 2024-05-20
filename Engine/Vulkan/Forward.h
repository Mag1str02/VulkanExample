#pragma once

#include "Engine/Utils/Base.h"

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
            using Interface::ImageResourceNode;
            using Interface::StaticResourceNode;

            using IEntry = Interface::Entry;
            using INode  = Interface::Node;

            using IPass        = Interface::Pass;
            using IPassFactory = Interface::PassFactory;
            using IPassNode    = Interface::PassNode;
            using IPassCluster = Interface::PassCluster;

            using IRemoveDependency  = Interface::RemovableDependency;
            using IProxyResourceNode = Interface::ProxyResourceNode;
            using IResourceNode      = Interface::ResourceNode;

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