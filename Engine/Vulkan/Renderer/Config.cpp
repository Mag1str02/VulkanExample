#include "Config.h"

#include <GLFW/glfw3.h>

namespace Engine::Vulkan {

    namespace {

        VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice device) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            return deviceProperties;
        }

        std::vector<const char*> ConvertToCStr(const std::unordered_set<std::string>& values) {
            std::vector<const char*> res;
            res.reserve(values.size());
            for (const auto& ext : values) {
                res.push_back(ext.c_str());
            }
            return res;
        }

    }  // namespace

    Config::Config() {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (size_t i = 0; i < glfwExtensionCount; ++i) {
            m_InstanceExtensions.insert(glfwExtensions[i]);
        }

#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        m_InstanceExtensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    }

    void Config::AddLayer(const std::string& layer) {
        m_Layers.insert(layer);
    }
    void Config::AddDeviceExtension(const std::string& extension) {
        m_DeviceExtensions.insert(extension);
    }
    void Config::AddInstanceExtension(const std::string& extension) {
        m_InstanceExtensions.insert(extension);
    }

    std::vector<const char*> Config::GetUsedInstanceExtensions() const {
        return ConvertToCStr(m_InstanceExtensions);
    }
    std::vector<const char*> Config::GetUsedDeviceExtensions() const {
        return ConvertToCStr(m_DeviceExtensions);
    }
    std::vector<const char*> Config::GetUsedLayers() const {
        return ConvertToCStr(m_Layers);
    }

    void Config::Validate() const {
        CheckLayersSupport();
        CheckInstanceExtensionSupport();
    }
    VkPhysicalDevice Config::ChooseDevice(VkInstance instance, const std::vector<VkPhysicalDevice>& devices) const {
        auto compatible = GetCompatibleDevices(instance, devices);
        DE_ASSERT(!compatible.empty(), "Failed to find compatible vulkan device");
        return ChooseBestDevice(compatible);
    }
    int32_t Config::GetUniversalQueueFamilyIndex(VkInstance instance, VkPhysicalDevice device) const {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
            const auto& props = queueFamilies[i];
            if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 || (props.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0 || props.queueCount < 2) {
                continue;
            }
            if (!glfwGetPhysicalDevicePresentationSupport(instance, device, i)) {
                continue;
            }
            return i;
        }

        return -1;
    }
    std::string GetDeviceName(VkPhysicalDevice device) {
        return GetDeviceProperties(device).deviceName;
    }

    void Config::CheckLayersSupport() const {
        auto unsupportedLayers = FindUnsupportedLayers();
        if (!unsupportedLayers.empty()) {
            for (const auto& unsupportedLayer : unsupportedLayers) {
                std::cerr << "Found unsupported layer: " << unsupportedLayer << std::endl;
            }
            DE_ASSERT(false, std::format("Found {} unsupported layers", unsupportedLayers.size()));
        }
    }
    void Config::CheckInstanceExtensionSupport() const {
        auto unsupportedExtensions = FindUnsupportedInstanceExtensions();
        if (!unsupportedExtensions.empty()) {
            for (const auto& ext : unsupportedExtensions) {
                std::cerr << "Found unsupported extension: " << ext << std::endl;
            }
            DE_ASSERT(false, std::format("Found {} unsupported extensions", unsupportedExtensions.size()));
        }
    }

    std::unordered_set<std::string> Config::FindUnsupportedLayers() const {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        std::unordered_set<std::string> unsupportedLayers(m_Layers.begin(), m_Layers.end());

        for (const auto& layerProperties : availableLayers) {
            unsupportedLayers.erase(layerProperties.layerName);
        }

        return unsupportedLayers;
    }
    std::unordered_set<std::string> Config::FindUnsupportedInstanceExtensions() const {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::unordered_set<std::string> unsupportedExtensions(m_InstanceExtensions.begin(), m_InstanceExtensions.end());
        for (const auto& ext : extensions) {
            unsupportedExtensions.erase(ext.extensionName);
        }

        return unsupportedExtensions;
    }
    std::unordered_set<std::string> Config::FindUnsupportedDeviceExtensions(VkPhysicalDevice device) const {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::unordered_set<std::string> unsupportedExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());
        for (const auto& extension : availableExtensions) {
            unsupportedExtensions.erase(extension.extensionName);
        }

        return unsupportedExtensions;
    }

    std::vector<VkPhysicalDevice> Config::GetCompatibleDevices(VkInstance instance, const std::vector<VkPhysicalDevice>& devices) const {
        std::vector<VkPhysicalDevice> compatibleDevices;
        for (const auto& device : devices) {
            if (CheckDevice(instance, device)) {
                compatibleDevices.push_back(device);
            }
        }
        return compatibleDevices;
    }
    bool Config::CheckDevice(VkInstance instance, VkPhysicalDevice device) const {
        std::println("Checking device {} ...", GetDeviceName(device));
        {
            if (GetUniversalQueueFamilyIndex(instance, device) == -1) {
                std::println("No universal queue");
                return false;
            }
        }
        {
            auto unsupportedExtensions = FindUnsupportedDeviceExtensions(device);
            if (!unsupportedExtensions.empty()) {
                std::println("Has unsupported extensions: {}", unsupportedExtensions);
                return false;
            }
        }
        std::println("Device Compatible");

        return true;
    }
    VkPhysicalDevice Config::ChooseBestDevice(const std::vector<VkPhysicalDevice>& devices) const {
        for (const auto& device : devices) {
            auto deviceProperties = GetDeviceProperties(device);
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                return device;
            }
        }
        return devices.front();
    }
}  // namespace Engine::Vulkan
