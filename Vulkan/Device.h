#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"
#include "Helpers.h"
#include "Queue.h"

class Window;

namespace Vulkan {
    class Instance;

    class Device : public std::enable_shared_from_this<Device> {
    public:
        NO_COPY_CONSTRUCTORS(Device);
        NO_MOVE_CONSTRUCTORS(Device);

        ~Device();

        VkDevice         GetLogicDevice();
        VkPhysicalDevice GetPhysicalDevice();
        Queue            GetQueue(Queue::Family queueFamily, uint32_t queueIndex);

        std::optional<uint32_t> GetFamilyIndex(Queue::Family family) const;

    private:
        friend class Instance;

        Device(VkPhysicalDevice device, Ref<Instance> instance, const QueuesSpecification& specification);
        uint32_t GetFamilySize(Queue::Family family) const;

        QueuesSpecification m_QueueSpecification;
        QueueFamilyIndices  m_QueueFamilyIndicies;
        VkPhysicalDevice    m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice            m_LogicDevice    = VK_NULL_HANDLE;

        Ref<Instance> m_Instance = nullptr;
    };

};  // namespace Vulkan