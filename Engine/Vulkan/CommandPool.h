#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {

    class CommandPool : public RefCounted<CommandPool>, NonCopyMoveable {
    public:
        static Ref<CommandPool> Create(Ref<Device> device, uint32_t family_index);
        ~CommandPool();

        const VkCommandPool& Handle();
        uint32_t             FamilyIndex();

        Ref<Device> GetDevice();

    private:
        CommandPool(Ref<Device> device, uint32_t familyIndex);

    private:
        friend class Device;

        uint32_t      m_FamilyIndex = 0;
        VkCommandPool m_Handle      = VK_NULL_HANDLE;
        Ref<Device>   m_Device      = nullptr;
    };

}  // namespace Engine::Vulkan