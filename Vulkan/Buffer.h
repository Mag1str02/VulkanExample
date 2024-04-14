#pragma once

#include "Common.h"
#include "Object.h"

namespace Vulkan {

    class Buffer : public Object {
    public:
        Buffer(Ref<Device> device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
        ~Buffer();

        void     SetData(void* data, uint64_t size);
        VkBuffer Handle();

    private:
        Ref<Device>           m_Device;
        VkBuffer              m_Buffer;
        VkDeviceMemory        m_Memory;
        uint64_t              m_Size;
        VkMemoryPropertyFlags m_MemoryProperties;
    };
}  // namespace Vulkan