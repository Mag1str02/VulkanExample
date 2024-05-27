#pragma once

#include "Engine/Vulkan/Interface/RawCommandBuffer.h"

namespace Engine::Vulkan::Managed {

    class RawCommandBuffer : public IRawCommandBuffer {
    public:
        RawCommandBuffer(VkCommandBuffer handle, bool secondary = false);
        RawCommandBuffer()          = default;
        virtual ~RawCommandBuffer() = default;

        virtual void Begin() override;
        virtual void End() override;
        virtual void Reset() override;

        virtual void AddImageMemoryBarrier(const VkImageMemoryBarrier2& barier) override;
        virtual void AddBufferMemoryBarrier(const VkBufferMemoryBarrier2& barier) override;
        virtual void FlushBariers() override;

        virtual void BeginRendering(const std::vector<Ref<ImageView>>& color_attachments) override;
        virtual void EndRendering() override;

        virtual void ExecuteCommandBuffer(IRawCommandBuffer& secondary_command_buffer) override;
        virtual void ClearImage(IImage& image, Vec4 clear_color) override;

        virtual const VkCommandBuffer& Handle() const override;
        virtual const bool             IsSecondary() const override;

    protected:
        void Init(VkCommandBuffer handle, bool secondary = false);

    private:
        VkCommandBuffer m_Handle = VK_NULL_HANDLE;

        std::vector<VkImageMemoryBarrier2>  m_ImageMemoryBariers;
        std::vector<VkBufferMemoryBarrier2> m_BufferMemoryBariers;
        bool                                m_Secondary;
    };

};  // namespace Engine::Vulkan::Managed