#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Interface {

    class CommandPool {
    public:
        virtual ~CommandPool() = default;

        virtual const VkCommandPool& Handle() const   = 0;
        virtual Queue*               GetQueue() const = 0;
    };
}  // namespace Engine::Vulkan::Interface