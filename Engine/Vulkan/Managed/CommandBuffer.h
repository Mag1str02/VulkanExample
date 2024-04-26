#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Managed {

    class CommandBuffer {
    public:
        virtual ~CommandBuffer();

        void AddImageMemoryBarier(VkImageMemoryBarrier2 barier);
        void AddBufferMemoryBarier(VkBufferMemoryBarrier2 barier);

        void BeginRendering(std::vector<Ref<ImageView>> color_attachments);
        void EndRendering();

        VkCommandBuffer Handle();

    protected:
        void ResetSecondaryCommandBuffers();
        void Init(VkCommandBuffer handle, Ref<CommandPool> pool);
        void FlushBariers();

    protected:
        VkCommandBuffer  m_Handle = VK_NULL_HANDLE;
        Ref<CommandPool> m_Pool;

    private:
        VkCommandPool   m_SecondaryCommandPool   = VK_NULL_HANDLE;
        VkCommandBuffer m_SecondaryCommandBuffer = VK_NULL_HANDLE;

        std::vector<VkImageMemoryBarrier2>  m_ImageMemoryBariers;
        std::vector<VkBufferMemoryBarrier2> m_BufferMemoryBariers;
    };

};  // namespace Engine::Vulkan::Managed