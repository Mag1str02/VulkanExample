#include "Image.h"

#include "Device.h"
#include "Helpers.h"

namespace Engine::Vulkan {

    Image::Image(Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) :
        m_Device(device), m_Width(width), m_Height(height) {
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

        VK_CHECK(vkCreateImage(m_Device->GetLogicDevice(), &imageInfo, nullptr, &m_Image));

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device->GetLogicDevice(), m_Image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = Helpers::FindMemoryType(memRequirements.memoryTypeBits, properties, m_Device->GetPhysicalDevice());

        VK_CHECK(vkAllocateMemory(m_Device->GetLogicDevice(), &allocInfo, nullptr, &m_Memory));
        VK_CHECK(vkBindImageMemory(m_Device->GetLogicDevice(), m_Image, m_Memory, 0));
    }
    Image::~Image() {
        vkFreeMemory(m_Device->GetLogicDevice(), m_Memory, nullptr);
        vkDestroyImage(m_Device->GetLogicDevice(), m_Image, nullptr);
    }

    VkImage Image::Handle() {
        return m_Image;
    }
    Ref<Device> Image::GetDevice() {
        return m_Device;
    }

}  // namespace Vulkan