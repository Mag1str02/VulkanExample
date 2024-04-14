#include "Buffer.h"

#include "Device.h"
#include "Helpers.h"

namespace Vulkan {

    Buffer::Buffer(Ref<Device> device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :
        m_Device(device), m_Size(size), m_MemoryProperties(properties) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = size;
        bufferInfo.usage       = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK(vkCreateBuffer(device->GetLogicDevice(), &bufferInfo, nullptr, &m_Buffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->GetLogicDevice(), m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = Helpers::FindMemoryType(memRequirements.memoryTypeBits, properties, m_Device->GetPhysicalDevice());

        VK_CHECK(vkAllocateMemory(m_Device->GetLogicDevice(), &allocInfo, nullptr, &m_Memory));
        VK_CHECK(vkBindBufferMemory(m_Device->GetLogicDevice(), m_Buffer, m_Memory, 0))
    }
    Buffer::~Buffer() {
        vkFreeMemory(m_Device->GetLogicDevice(), m_Memory, nullptr);
        vkDestroyBuffer(m_Device->GetLogicDevice(), m_Buffer, nullptr);
    }

    void Buffer::SetData(void* data, uint64_t size) {
        DE_ASSERT(m_MemoryProperties | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                  "Cannot set data on buffer without VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT memory property");
        DE_ASSERT(size <= m_Size, "Set data size exceeds the size of buffer");

        void* mapped_memory;
        vkMapMemory(m_Device->GetLogicDevice(), m_Memory, 0, size, 0, &mapped_memory);
        memcpy(mapped_memory, data, size);
        vkUnmapMemory(m_Device->GetLogicDevice(), m_Memory);
    }

    VkBuffer Buffer::Handle() {
        return m_Buffer;
    }

}  // namespace Vulkan