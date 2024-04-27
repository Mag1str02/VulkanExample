#include "Helpers.h"

namespace Engine::Vulkan::Helpers {

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        DE_ASSERT(false, "Failed to find suitable memory type!");
        return 0;
    }

    VkImageAspectFlags ImageAspectFlagsFromFormat(VkFormat format) {
        switch (format) {
            case VK_FORMAT_B8G8R8A8_SRGB: return VK_IMAGE_ASPECT_COLOR_BIT;
            default: DE_ASSERT(false, std::format("Image format {} not supported", (uint32_t)format)); return VK_FORMAT_UNDEFINED;
        };
    }

}  // namespace Engine::Vulkan::Helpers