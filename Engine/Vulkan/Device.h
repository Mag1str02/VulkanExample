#pragma once

#include "Queue.h"

namespace Engine::Vulkan {

    class Device : public RefCounted<Device> {
    public:
        static Ref<Device> Create(VkPhysicalDevice device, Ref<Instance> instance, const Config& config);

        ~Device();

        VkDevice         GetLogicDevice();
        VkPhysicalDevice GetPhysicalDevice();
        tracy::VkCtx*    GetTracyCtx();

        Ref<Queue> GetQueue();

    private:
        Device(VkPhysicalDevice device, Ref<Instance> instance, const Config& config);

    private:
        tracy::VkCtx* m_TracyContext;

        ManualLifetime<Queue> m_Queue;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         m_LogicDevice    = VK_NULL_HANDLE;

        Ref<Instance> m_Instance = nullptr;
    };

};  // namespace Engine::Vulkan