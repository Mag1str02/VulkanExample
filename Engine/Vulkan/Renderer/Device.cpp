#include "Device.h"

#include "Instance.h"

namespace Engine::Vulkan {

    Ref<Device> Device::Create(VkPhysicalDevice device, Ref<Instance> instance, const Config& config) {
        return Ref<Device>(new Device(device, instance, config));
    }

    Device::Device(VkPhysicalDevice device, Ref<Instance> instance, const Config& config) : m_PhysicalDevice(device), m_Instance(instance) {
        DE_ASSERT(m_Instance, "Bad instance");

        auto requiredExtensions = config.GetUsedDeviceExtensions();
        auto requiredLayers     = config.GetUsedLayers();

        int32_t universal_queue_family_index = config.GetUniversalQueueFamilyIndex(m_Instance->Handle(), m_PhysicalDevice);
        DE_ASSERT(universal_queue_family_index != -1, "No universal queue family");

        float                   priorities[2] = {0.5f, 0.5f};
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = universal_queue_family_index;
        queueCreateInfo.queueCount       = 2;
        queueCreateInfo.pQueuePriorities = priorities;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkPhysicalDeviceHostQueryResetFeatures hostQueryFeature{};
        hostQueryFeature.sType          = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
        hostQueryFeature.hostQueryReset = VK_TRUE;

        VkPhysicalDeviceSynchronization2Features synchronization2Feature{};
        synchronization2Feature.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
        synchronization2Feature.synchronization2 = VK_TRUE;
        synchronization2Feature.pNext            = &hostQueryFeature;

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

        VkQueue graphic_queue;
        VkQueue presentation_queue;
        vkGetDeviceQueue(m_LogicDevice, universal_queue_family_index, 0, &graphic_queue);
        vkGetDeviceQueue(m_LogicDevice, universal_queue_family_index, 1, &presentation_queue);

        m_GraphicsQueue.Construct(this, graphic_queue, universal_queue_family_index);
        m_PresentationQueue.Construct(this, presentation_queue, universal_queue_family_index);

        // TODO: Validate that tracy extensions and features available
        m_TracyContext = TracyVkContextHostCalibrated(m_PhysicalDevice,
                                                      m_LogicDevice,
                                                      vkResetQueryPool,
                                                      vkGetPhysicalDeviceCalibrateableTimeDomainsEXT,
                                                      vkGetCalibratedTimestampsEXT);
    }

    Device::~Device() {
        TracyVkDestroy(m_TracyContext);
        m_GraphicsQueue.Destruct();
        m_PresentationQueue.Destruct();
        vkDestroyDevice(m_LogicDevice, nullptr);
    }

    void Device::WaitIdle() {
        VK_CHECK(vkDeviceWaitIdle(m_LogicDevice));
    }
    VkDevice Device::GetLogicDevice() {
        return m_LogicDevice;
    }
    VkPhysicalDevice Device::GetPhysicalDevice() {
        return m_PhysicalDevice;
    }

    Ref<Instance> Device::GetInstance() {
        return m_Instance;
    }
    Ref<GraphicsQueue> Device::GetGraphicsQueue() {
        return Ref<GraphicsQueue>(shared_from_this(), m_GraphicsQueue.Get());
    }
    Ref<PresentationQueue> Device::GetPresentationQueue() {
        return Ref<PresentationQueue>(shared_from_this(), m_PresentationQueue.Get());
    }
    tracy::VkCtx* Device::GetTracyCtx() {
        return m_TracyContext;
    }

}  // namespace Engine::Vulkan