#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"
#include "Helpers.h"

class Window;

namespace Vulkan {

    class Device : public std::enable_shared_from_this<Device> {
    public:
        NO_COPY_CONSTRUCTORS(Device);
        NO_MOVE_CONSTRUCTORS(Device);

        ~Device();

        VkDevice                GetLogicDevice();
        VkPhysicalDevice        GetPhysicalDevice();
        Queue                   GetQueue(QueueFamily queueFamily, uint32_t queueIndex);
        std::optional<uint32_t> GetFamilyIndex(QueueFamily family) const;

        Ref<Pipeline> CreatePipeline(const PipelineSpecification& spec);

    private:
        friend class Instance;

        Device(VkPhysicalDevice device, Ref<Instance> instance, const QueuesSpecification& specification);
        uint32_t GetFamilySize(QueueFamily family) const;

        QueuesSpecification m_QueueSpecification;
        QueueFamilyIndices  m_QueueFamilyIndicies;
        VkPhysicalDevice    m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice            m_LogicDevice    = VK_NULL_HANDLE;

        Ref<Instance> m_Instance = nullptr;
    };

};  // namespace Vulkan