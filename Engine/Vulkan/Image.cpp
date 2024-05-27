#include "Image.h"

#include "Engine/Vulkan/Helpers.h"
#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan {
    Ref<Image> Image::Create(
        Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
        return Ref<Image>(new Image(device, width, height, format, usage, properties));
    }

    Image::Image(Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::Image::Image");
        VkImageCreateInfo imageInfo{};
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = format;
        imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage         = usage;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;

        VkImage handle;
        VK_CHECK(vkCreateImage(device->GetLogicDevice(), &imageInfo, nullptr, &handle));

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device->GetLogicDevice(), handle, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = Helpers::FindMemoryType(memRequirements.memoryTypeBits, properties, device->GetPhysicalDevice());

        VkDeviceMemory memory;
        VK_CHECK(vkAllocateMemory(device->GetLogicDevice(), &allocInfo, nullptr, &memory));
        VK_CHECK(vkBindImageMemory(device->GetLogicDevice(), handle, memory, 0));

        Init(handle, format, usage, {.width = width, .height = height}, device.get());
    }
    Image::~Image() {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::Image::~Image");
        vkFreeMemory(m_Device->GetLogicDevice(), m_Memory, nullptr);
        vkDestroyImage(m_Device->GetLogicDevice(), m_Image, nullptr);
    }

}  // namespace Engine::Vulkan