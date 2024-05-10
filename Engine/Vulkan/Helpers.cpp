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

        DE_ASSERT_FAIL("Failed to find suitable memory type!");
        return 0;
    }

    VkImageAspectFlags ImageAspectFlagsFromFormat(VkFormat format) {
        switch (format) {
            case VK_FORMAT_B8G8R8A8_SRGB: return VK_IMAGE_ASPECT_COLOR_BIT;
            default: DE_ASSERT_FAIL("Image format {} not supported", (uint32_t)format);
        };
    }

}  // namespace Engine::Vulkan::Helpers