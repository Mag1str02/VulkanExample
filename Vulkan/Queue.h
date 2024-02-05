#pragma once

#include <volk.h>

#include "Vulkan/Common.h"

namespace Vulkan {

    class Queue {
    public:
        ~Queue() = default;

        const VkQueue& Handle();
        uint32_t       FamilyIndex() const;

    private:
        Queue(VkQueue handle, uint32_t familyIndex);
        void Invalidate();

    private:
        friend class Device;

        std::mutex m_Mutex;
        VkQueue    m_Handle      = VK_NULL_HANDLE;
        uint32_t   m_FamilyIndex = 0;
        bool       m_Valid       = true;
    };

}  // namespace Vulkan