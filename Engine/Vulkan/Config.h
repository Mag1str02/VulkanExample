#pragma once

#include "Common.h"

namespace Engine::Vulkan {

    class Config {
    public:
        Config();

        void AddLayer(const std::string& layer);
        void AddDeviceExtension(const std::string& extension);
        void AddInstanceExtension(const std::string& extension);

        std::vector<const char*> GetUsedInstanceExtensions() const;
        std::vector<const char*> GetUsedDeviceExtensions() const;
        std::vector<const char*> GetUsedLayers() const;

        void             Validate() const;
        VkPhysicalDevice ChooseDevice(VkInstance instance, const std::vector<VkPhysicalDevice>& devices) const;
        int32_t          GetUniversalQueueFamilyIndex(VkInstance instance, VkPhysicalDevice device) const;

    private:
        void CheckInstanceExtensionSupport() const;
        void CheckLayersSupport() const;

        std::unordered_set<std::string> FindUnsupportedInstanceExtensions() const;
        std::unordered_set<std::string> FindUnsupportedLayers() const;
        std::unordered_set<std::string> FindUnsupportedDeviceExtensions(VkPhysicalDevice device) const;

        bool                          CheckDevice(VkInstance instance, VkPhysicalDevice device) const;
        std::vector<VkPhysicalDevice> GetCompatibleDevices(VkInstance instance, const std::vector<VkPhysicalDevice>& devices) const;
        VkPhysicalDevice              ChooseBestDevice(const std::vector<VkPhysicalDevice>& devices) const;

    private:
        std::unordered_set<std::string> m_Layers;
        std::unordered_set<std::string> m_InstanceExtensions;
        std::unordered_set<std::string> m_DeviceExtensions;
    };

}  // namespace Engine::Vulkan