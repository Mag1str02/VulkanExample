#pragma once

#include "Engine/Vulkan/Managed/CommandPool.h"

namespace Engine::Vulkan {

    class CommandPool : public Managed::CommandPool, NonCopyMoveable {
    public:
        static Ref<CommandPool> Create(Ref<Queue> queue, bool transient = false);
        ~CommandPool();

    private:
        CommandPool(Ref<Queue> queue, bool transient);

    private:
        Ref<Queue> m_Queue;
    };

}  // namespace Engine::Vulkan