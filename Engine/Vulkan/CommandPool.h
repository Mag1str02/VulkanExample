#pragma once

#include "Common.h"

namespace Engine::Vulkan {

    class CommandPool : public RefCounted<CommandPool> {
    public:
        ~CommandPool();

        Ref<CommandBuffer> CreateCommandBuffer();

        const VkCommandPool& Handle();
        uint32_t             FamilyIndex();

        Ref<Device> GetDevice();

        NO_COPY_CONSTRUCTORS(CommandPool);
        NO_MOVE_CONSTRUCTORS(CommandPool);

    private:
        CommandPool(Ref<Device> device, uint32_t familyIndex);

    private:
        friend class Device;

        uint32_t      m_FamilyIndex = 0;
        VkCommandPool m_Handle      = VK_NULL_HANDLE;
        Ref<Device>   m_Device      = nullptr;
    };

}  // namespace Vulkan