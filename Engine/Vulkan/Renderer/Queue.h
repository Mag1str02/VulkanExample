#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class Queue : NonCopyMoveable {
    public:
        Queue(Device* device, VkQueue queue, uint32_t queue_family_index);
        virtual ~Queue();

        Ref<Queue> CreateRef();

        void WaitIdle() const;

        Device*        GetDevice() const;
        uint32_t       FamilyIndex() const;
        const VkQueue& Handle() const;

    protected:
        Device* const  m_Device;
        const VkQueue  m_Queue;
        const uint32_t m_FamilyIndex;
    };

}  // namespace Engine::Vulkan