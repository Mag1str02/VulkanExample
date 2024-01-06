#include "Device.h"

#include "Vulkan/Helpers.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/Queue.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/Window.h"

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

        for (const auto& [familyIndex, amount] : actualAmounts) {
            for (uint32_t i = 0; i < amount; ++i) {
                VkQueue handle;
                vkGetDeviceQueue(m_LogicDevice, familyIndex, i, &handle);
                m_Queues[familyIndex].emplace_back(Ref<Queue>(new Queue(handle, familyIndex)));
            }
        }
    }

    Device::~Device() {
        for (const auto& [_, queues] : m_Queues) {
            for (const auto& queue : queues) {
                queue->Invalidate();
            }
        }
        vkDestroyDevice(m_LogicDevice, nullptr);
    }

    const VkDevice& Device::GetLogicDevice() {
        return m_LogicDevice;
    }
    const VkPhysicalDevice& Device::GetPhysicalDevice() {
        return m_PhysicalDevice;
    }
    Ref<Queue> Device::GetQueue(QueueFamily queueFamily, uint32_t queueIndex) {
        auto familySize  = GetFamilySize(queueFamily);
        auto familyIndex = m_QueueFamilyIndicies.GetFamilyIndex(queueFamily);
        DE_ASSERT(familyIndex, "Bad family");
        DE_ASSERT(queueIndex < familySize, "Bad queue index");

        return m_Queues.at(*familyIndex).at(queueIndex);
    }
    Ref<Instance> Device::GetInstance() {
        return m_Instance;
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

    Ref<SwapChain> Device::CreateSwapChain(Ref<Window> window) {
        return Ref<SwapChain>(new SwapChain(window, shared_from_this()));
    }
    Ref<Pipeline> Device::CreatePipeline(const PipelineSpecification& spec) {
        return Ref<Pipeline>(new Pipeline(shared_from_this(), spec));
    }
    Ref<CommandPool> Device::CreateCommandPool(QueueFamily family) {
        auto familyIndex = GetFamilyIndex(family);
        DE_ASSERT(familyIndex, "Bad family");
        return Ref<CommandPool>(new CommandPool(shared_from_this(), *familyIndex));
    }
}  // namespace Vulkan