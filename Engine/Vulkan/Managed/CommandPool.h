#pragma once

#include "Engine/Vulkan/Interface/CommandPool.h"

namespace Engine::Vulkan::Managed {

    class CommandPool : public ICommandPool {
    public:
        CommandPool(Queue* queue, VkCommandPool handle);
        virtual ~CommandPool() = default;

        virtual const VkCommandPool& Handle() const override;
        virtual Queue*               GetQueue() const override;

    protected:
        CommandPool() = default;
        void Init(Queue* queue, VkCommandPool handle);

    private:
        VkCommandPool m_Handle;
        Queue*        m_Queue;
    };
}  // namespace Engine::Vulkan::Managed