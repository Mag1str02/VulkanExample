#include "Instance.h"

#include "Debugger.h"
#include "Device.h"
#include "Helpers.h"
#include "Window.h"

namespace Engine::Vulkan {

    Ref<Instance> Instance::Create(const Config& config) {
        return Ref<Instance>(new Instance(config));
    }

    Instance::Instance(const Config& config) {
        config.Validate();

        auto requiredExtensions = config.GetUsedInstanceExtensions();
        auto requiredLayers     = config.GetUsedLayers();

        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "No Engine";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo        = &appInfo;
        createInfo.enabledLayerCount       = requiredLayers.size();
        createInfo.ppEnabledLayerNames     = requiredLayers.data();
        createInfo.enabledExtensionCount   = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        auto debugCreateInfo      = Debugger::GenCreateInfo();
        debugCreateInfo.pUserData = this;
        createInfo.pNext          = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

        auto result = vkCreateInstance(&createInfo, nullptr, &m_Handle);
        DE_ASSERT(result == VK_SUCCESS, "Failed to create instance");
        volkLoadInstance(m_Handle);
    }

    Instance::~Instance() {
        vkDestroyInstance(m_Handle, nullptr);
    }

    VkInstance Instance::Handle() {
        return m_Handle;
    }

    std::vector<VkPhysicalDevice> Instance::GetAllDevices() {
        uint32_t                      deviceCount = GetDeviceCount();
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Handle, &deviceCount, devices.data());
        return devices;
    }

    uint32_t Instance::GetDeviceCount() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Handle, &deviceCount, nullptr);
        return deviceCount;
    }

}  // namespace Engine::Vulkan
