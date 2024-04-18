#include "Device.h"

#include "CommandPool.h"
#include "Helpers.h"
#include "Instance.h"
#include "Window.h"

namespace Engine::Vulkan {

    Device::Device(VkPhysicalDevice device, Instance* instance, const Config& config) : m_PhysicalDevice(device), m_Instance(instance) {
        DE_ASSERT(instance, "Bad instance");

        auto requiredExtensions = config.GetUsedDeviceExtensions();
        auto requiredLayers     = config.GetUsedLayers();

        int32_t universal_queue_family_index = config.GetUniversalQueueFamilyIndex(instance->Handle(), m_PhysicalDevice);
        DE_ASSERT(universal_queue_family_index != -1, "No universal queue family");

        float                   priority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = universal_queue_family_index;
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &priority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkPhysicalDeviceSynchronization2Features synchronization2Feature{};
        synchronization2Feature.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
        synchronization2Feature.synchronization2 = true;

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{};
        dynamicRenderingFeature.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamicRenderingFeature.pNext            = &synchronization2Feature;
        dynamicRenderingFeature.dynamicRendering = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount    = 1;
        createInfo.pQueueCreateInfos       = &queueCreateInfo;
        createInfo.pEnabledFeatures        = &deviceFeatures;
        createInfo.enabledLayerCount       = requiredLayers.size();
        createInfo.ppEnabledLayerNames     = requiredLayers.data();
        createInfo.enabledExtensionCount   = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.pNext                   = &dynamicRenderingFeature;

        VK_CHECK(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicDevice));

        m_QueueFamilyIndex = universal_queue_family_index;
        vkGetDeviceQueue(m_LogicDevice, m_QueueFamilyIndex, 0, &m_Queue);
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
    VkQueue Device::GetQueue() {
        return m_Queue;
    }
    uint32_t Device::GetQueueFamilyIndex() {
        return m_QueueFamilyIndex;
    }

}  // namespace Engine::Vulkan