#pragma once

#include "Common.h"

namespace Engine::Vulkan::Helpers {

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device);

    VkImageAspectFlags ImageAspectFlagsFromFormat(VkFormat format);

}  // namespace Engine::Vulkan::Helpers