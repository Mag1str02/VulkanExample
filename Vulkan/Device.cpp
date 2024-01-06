#include "Device.h"

#include "Helpers.h"
#include "Pipeline.h"
#include "Queue.h"
#include "Window.h"

namespace Vulkan {

    Device::Device(VkPhysicalDevice device, Ref<Instance> instance, const QueuesSpecification& specification) : m_QueueSpecification(specification) {
        DE_ASSERT(instance, "Bad instance");

        m_Instance            = instance;
        m_PhysicalDevice      = device;
        m_QueueFamilyIndicies = Helpers::GetDeviceQueueFamilies(m_Instance->Handle(), m_PhysicalDevice);

        float priority           = 1.0f;
        auto  requiredExtensions = Helpers::GetRequiredDeviceExtensions();
        auto  requiredLayers     = Helpers::GetRequiredLayers();
        auto  uniqueQueues       = m_QueueFamilyIndicies.GetUniqueIndicies();

        std::unordered_map<uint32_t, uint32_t> actualAmounts;
        for (const auto& [family, index] : m_QueueFamilyIndicies.GetFamilies()) {
            auto requestedFamilySize = GetFamilySize(family);
            if (requestedFamilySize) {
                actualAmounts[index] = std::max(actualAmounts[index], requestedFamilySize);
            }
        }

        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;
        for (const auto& [familyIndex, amount] : actualAmounts) {
            VkDeviceQueueCreateInfo createInfo{};
            createInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = familyIndex;
            createInfo.queueCount       = amount;
            createInfo.pQueuePriorities = &priority;
            queuesCreateInfo.push_back(createInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature{};
        dynamicRenderingFeature.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamicRenderingFeature.dynamicRendering = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos       = queuesCreateInfo.data();
        createInfo.queueCreateInfoCount    = uniqueQueues.size();
        createInfo.pEnabledFeatures        = &deviceFeatures;
        createInfo.enabledLayerCount       = requiredLayers.size();
        createInfo.ppEnabledLayerNames     = requiredLayers.data();
        createInfo.enabledExtensionCount   = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.pNext                   = &dynamicRenderingFeature;

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
    Queue Device::GetQueue(QueueFamily queueFamily, uint32_t queueIndex) {
        auto familySize  = GetFamilySize(queueFamily);
        auto familyIndex = m_QueueFamilyIndicies.GetFamilyIndex(queueFamily);
        DE_ASSERT(familyIndex, "Bad family");
        DE_ASSERT(queueIndex < familySize, "Bad queue index");

        VkQueue handle;
        vkGetDeviceQueue(m_LogicDevice, *familyIndex, queueIndex, &handle);
        return Queue(shared_from_this(), handle);
    }

    std::optional<uint32_t> Device::GetFamilyIndex(QueueFamily family) const {
        return m_QueueFamilyIndicies.GetFamilyIndex(family);
    }

    uint32_t Device::GetFamilySize(QueueFamily family) const {
        if (auto it = m_QueueSpecification.find(family); it != m_QueueSpecification.end()) {
            return it->second;
        }
        return 0;
    }

    Ref<Pipeline> Device::CreatePipeline(const PipelineSpecification& spec) {
        return Ref<Pipeline>(new Pipeline(shared_from_this(), spec));
    }
}  // namespace Vulkan