#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"

namespace Vulkan::Helpers {

    struct QueueFamilyIndices {
        std::optional<uint32_t> m_GraphicsFamily;
        std::optional<uint32_t> m_PresentFamily;

        std::unordered_set<uint32_t> GetUniqueQueueIndicies() const;
    };

    std::vector<const char*> GetRequiredLayers();
    std::vector<const char*> GetRequiredInstanceExtensions();
    std::vector<const char*> GetRequiredDeviceExtensions();

    void CheckLayersSupport();
    void CheckInstanceExtensionSupport();
    bool CheckDevice(VkInstance instance, VkPhysicalDevice device);

    QueueFamilyIndices GetDeviceQueueFamilies(VkInstance instance, VkPhysicalDevice device);

}  // namespace Vulkan::Helpers