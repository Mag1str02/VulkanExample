#pragma once

#include <optional>
#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Managed {

    class CommandBuffer {
    public:
        virtual ~CommandBuffer();

        void AddImageMemoryBarrier(const VkImageMemoryBarrier2& barier);
        void AddBufferMemoryBarrier(const VkBufferMemoryBarrier2& barier);

        void BeginRendering(const std::vector<Ref<ImageView>>& color_attachments);
        void EndRendering();
        void ClearImage(Ref<IImage> image, Vec4 clear_color);

        const VkCommandBuffer& Handle();

    protected:
        CommandBuffer() = default;

        void ResetSecondaryCommandBuffers();
        void Init(VkCommandBuffer handle, Ref<CommandPool> pool);
        void FlushBariers();

    protected:
        VkCommandBuffer  m_Handle = VK_NULL_HANDLE;
        Ref<CommandPool> m_Pool;

    private:
        VkCommandPool                m_SecondaryCommandPool         = VK_NULL_HANDLE;
        VkCommandBuffer              m_CurrentSecondaryCommandBufer = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_SecondaryCommandBuffers;

        std::vector<VkImageMemoryBarrier2>  m_ImageMemoryBariers;
        std::vector<VkBufferMemoryBarrier2> m_BufferMemoryBariers;
    };

};  // namespace Engine::Vulkan::Managed