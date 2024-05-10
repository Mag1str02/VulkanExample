#pragma once

#include "GraphicsQueue.h"
#include "PresentationQueue.h"

namespace Engine::Vulkan {

    class Device : public RefCounted<Device> {
    public:
        static Ref<Device> Create(VkPhysicalDevice device, Ref<Instance> instance, const Config& config);

        ~Device();

        VkDevice         GetLogicDevice();
        VkPhysicalDevice GetPhysicalDevice();
        tracy::VkCtx*    GetTracyCtx();

        Ref<Instance>          GetInstance();
        Ref<GraphicsQueue>     GetGraphicsQueue();
        Ref<PresentationQueue> GetPresentationQueue();

    private:
        Device(VkPhysicalDevice device, Ref<Instance> instance, const Config& config);

    private:
        tracy::VkCtx* m_TracyContext;

        ManualLifetime<GraphicsQueue>     m_GraphicsQueue;
        ManualLifetime<PresentationQueue> m_PresentationQueue;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         m_LogicDevice    = VK_NULL_HANDLE;

        Ref<Instance> m_Instance;
    };

};  // namespace Engine::Vulkan