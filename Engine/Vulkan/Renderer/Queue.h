#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Queue : NonCopyMoveable {
    public:
        Queue(Device* device, VkQueue queue, uint32_t queue_family_index);
        virtual ~Queue();

        void WaitIdle();

        Ref<Device> GetDevice();
        uint32_t    FamilyIndex();
        VkQueue     Handle();

    protected:
        Device* const  m_Device;
        const VkQueue  m_Queue;
        const uint32_t m_FamilyIndex;
    };

}  // namespace Engine::Vulkan