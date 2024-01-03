#include "Device.h"

#include "Helpers.h"
#include "Window.h"

namespace Vulkan {

    Device::Device(VkPhysicalDevice device, Ref<Instance> instance) {
        DE_ASSERT(instance, "Bad instance");

        m_Instance       = instance;
        m_PhysicalDevice = device;

        float priority           = 1.0f;
        auto  queueFamilies      = Helpers::GetDeviceQueueFamilies(m_Instance->Handle(), m_PhysicalDevice);
        auto  requiredExtensions = Helpers::GetRequiredDeviceExtensions();
        auto  requiredLayers     = Helpers::GetRequiredLayers();
        auto  uniqueQueues       = queueFamilies.GetUniqueQueueIndicies();

        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;
        for (const auto& queue : uniqueQueues) {
            VkDeviceQueueCreateInfo createInfo{};
            createInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = queue;
            createInfo.queueCount       = 1;
            createInfo.pQueuePriorities = &priority;
            queuesCreateInfo.push_back(createInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos       = queuesCreateInfo.data();
        createInfo.queueCreateInfoCount    = uniqueQueues.size();
        createInfo.pEnabledFeatures        = &deviceFeatures;
        createInfo.enabledLayerCount       = requiredLayers.size();
        createInfo.ppEnabledLayerNames     = requiredLayers.data();
        createInfo.enabledExtensionCount   = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        auto res = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicDevice);
        DE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to create logic device");
    }

    Device::~Device() {
        vkDestroyDevice(m_LogicDevice, nullptr);
    }

    VkDevice Device::GetLogicDevice() {
        return m_LogicDevice;
    }
    VkPhysicalDevice Device::GetPhysicalDevice() {
        return m_PhysicalDevice;
    }
}  // namespace Vulkan