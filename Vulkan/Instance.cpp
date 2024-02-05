#include "Instance.h"

#include "Vulkan/Helpers.h"
#include "Vulkan/Debugger.h"
#include "Vulkan/Device.h"
#include "Vulkan/Window.h"

namespace Vulkan {

    Ref<Instance> Instance::Create() {
        Helpers::CheckLayersSupport();
        Helpers::CheckInstanceExtensionSupport();

        Ref<Instance> res(new Instance());

        auto requiredExtensions = Helpers::GetRequiredInstanceExtensions();
        auto requiredLayers     = Helpers::GetRequiredLayers();

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
        debugCreateInfo.pUserData = res.get();
        createInfo.pNext          = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

        auto result = vkCreateInstance(&createInfo, nullptr, &res->m_Handle);
        DE_ASSERT(result == VK_SUCCESS, "Failed to create instance");
        volkLoadInstance(res->m_Handle);
        return res;
    }

    Instance::~Instance() {
        vkDestroyInstance(m_Handle, nullptr);
    }

    const VkInstance& Instance::Handle() {
        return m_Handle;
    }

    Ref<Device> Instance::CreateBestDevice(const QueuesSpecification& specification) {
        auto devices = GetCompatibleDevices(specification);
        DE_ASSERT(!devices.empty(), "Failed to find compatible vulkan device");
        return Ref<Device>(new Device(devices.front(), shared_from_this(), specification));
    }
    Ref<Debugger> Instance::CreateDebugger() {
        return Ref<Debugger>(new Debugger(shared_from_this()));
    }

    uint32_t Instance::GetDeviceCount() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Handle, &deviceCount, nullptr);
        return deviceCount;
    }

    std::vector<VkPhysicalDevice> Instance::GetAllDevices() {
        uint32_t                      deviceCount = GetDeviceCount();
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Handle, &deviceCount, devices.data());
        return devices;
    }

    std::vector<VkPhysicalDevice> Instance::GetCompatibleDevices(const QueuesSpecification& specification) {
        std::vector<VkPhysicalDevice> compatibleDevices;
        auto                          allDevices = GetAllDevices();
        for (const auto& device : allDevices) {
            if (Helpers::CheckDevice(m_Handle, device, specification)) {
                compatibleDevices.push_back(device);
            }
        }
        return compatibleDevices;
    }

}  // namespace Vulkan
