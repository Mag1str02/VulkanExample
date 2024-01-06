#pragma once

#include <vulkan/vulkan.h>

#include "Vulkan/Common.h"

class Window;

namespace Vulkan {

    class Device : public RefCounted<Device> {
    public:
        ~Device();

        const VkDevice&         GetLogicDevice();
        const VkPhysicalDevice& GetPhysicalDevice();

        std::optional<uint32_t> GetFamilyIndex(QueueFamily family) const;
        Ref<Queue>              GetQueue(QueueFamily queueFamily, uint32_t queueIndex);
        Ref<Instance>           GetInstance();

        Ref<SwapChain>   CreateSwapChain(Ref<Window> window);
        Ref<Pipeline>    CreatePipeline(const PipelineSpecification& spec);
        Ref<CommandPool> CreateCommandPool(QueueFamily family);

        NO_COPY_CONSTRUCTORS(Device);
        NO_MOVE_CONSTRUCTORS(Device);

    private:
        Device(VkPhysicalDevice device, Ref<Instance> instance, const QueuesSpecification& specification);
        uint32_t GetFamilySize(QueueFamily family) const;

    private:
        friend class Instance;
        using Queues = std::unordered_map<uint32_t, std::vector<Ref<Queue>>>;

        QueuesSpecification m_QueueSpecification;
        QueueFamilyIndices  m_QueueFamilyIndicies;
        Queues              m_Queues;
        VkPhysicalDevice    m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice            m_LogicDevice    = VK_NULL_HANDLE;

        Ref<Instance> m_Instance = nullptr;
    };

};  // namespace Vulkan