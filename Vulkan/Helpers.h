#pragma once

#include <volk.h>

#include "Vulkan/Common.h"

namespace Vulkan::Helpers {

    std::vector<const char*> GetRequiredLayers();
    std::vector<const char*> GetRequiredInstanceExtensions();
    std::vector<const char*> GetRequiredDeviceExtensions();

    void CheckLayersSupport();
    void CheckInstanceExtensionSupport();
    bool CheckDevice(VkInstance instance, VkPhysicalDevice device, const QueuesSpecification& specification);

    QueueFamilyIndices GetDeviceQueueFamilies(VkInstance instance, VkPhysicalDevice device);

    SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

    void     InsertImageMemoryBarier(VkCommandBuffer         buffer,
                                     VkImage                 image,
                                     VkImageLayout           oldLayout,
                                     VkImageLayout           newLayout,
                                     VkPipelineStageFlagBits srcStage,
                                     VkPipelineStageFlagBits dstStage);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device);

}  // namespace Vulkan::Helpers