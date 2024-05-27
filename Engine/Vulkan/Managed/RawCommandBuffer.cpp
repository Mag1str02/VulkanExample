#include "RawCommandBuffer.h"

#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/ImageView.h"
#include "Engine/Vulkan/Interface/Image.h"
#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan::Managed {

    RawCommandBuffer::RawCommandBuffer(VkCommandBuffer handle, bool secondary) : m_Handle(handle), m_Secondary(secondary) {}

    void RawCommandBuffer::Init(VkCommandBuffer handle, bool secondary) {
        m_Handle    = handle;
        m_Secondary = secondary;
    }

    void RawCommandBuffer::Begin() {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::Begin");

        VkCommandBufferInheritanceInfo inheritance_info{};
        inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (m_Secondary) {
            beginInfo.pNext = &inheritance_info;
        }

        VK_CHECK(vkBeginCommandBuffer(m_Handle, &beginInfo));
    }
    void RawCommandBuffer::End() {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::End");
        FlushBariers();
        VK_CHECK(vkEndCommandBuffer(m_Handle));
    }
    void RawCommandBuffer::Reset() {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::Reset");
        VK_CHECK(vkResetCommandBuffer(m_Handle, 0));
    }

    void RawCommandBuffer::BeginRendering(const std::vector<Ref<ImageView>>& color_attachments) {
        FlushBariers();
        PROFILER_SCOPE("Engine::Vulkan::Managed::RawCommandBuffer::BeginRendering");

        std::vector<VkRenderingAttachmentInfo> color_attachments_info;
        for (const auto& attachment : color_attachments) {
            VkRenderingAttachmentInfo attachment_info{};
            attachment_info.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachment_info.imageView   = attachment->Handle();
            attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachment_info.loadOp      = VK_ATTACHMENT_LOAD_OP_LOAD;
            attachment_info.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;

            color_attachments_info.emplace_back(std::move(attachment_info));
        }
        VkRenderingInfo info{};
        info.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO;
        info.renderArea.extent    = color_attachments[0]->GetExtent();
        info.layerCount           = 1;
        info.colorAttachmentCount = color_attachments.size();
        info.pColorAttachments    = color_attachments_info.data();

        vkCmdBeginRendering(m_Handle, &info);
    }
    void RawCommandBuffer::EndRendering() {
        PROFILER_SCOPE("Engine::Vulkan::Managed::RawCommandBuffer::EndRendering");

        vkCmdEndRendering(m_Handle);
    }
    void RawCommandBuffer::ExecuteCommandBuffer(IRawCommandBuffer& secondary_command_buffer) {
        DE_ASSERT(m_Secondary == false && secondary_command_buffer.IsSecondary(), "Only prime buffers can execute secondary command buffer");

        FlushBariers();
        vkCmdExecuteCommands(m_Handle, 1, &secondary_command_buffer.Handle());
    }

    void RawCommandBuffer::ClearImage(IImage& image, Vec4 clear_color) {
        PROFILER_SCOPE("Engine::Vulkan::Managed::RawCommandBuffer::ClearImage");
        FlushBariers();

        VkClearColorValue color;
        color.float32[0] = clear_color.r;
        color.float32[1] = clear_color.g;
        color.float32[2] = clear_color.b;
        color.float32[3] = clear_color.a;

        VkImageSubresourceRange range;
        range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;  // TODO: deduce from usage flags
        range.baseArrayLayer = 0;
        range.baseMipLevel   = 0;
        range.layerCount     = VK_REMAINING_MIP_LEVELS;
        range.levelCount     = VK_REMAINING_ARRAY_LAYERS;

        vkCmdClearColorImage(m_Handle, image.Handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);
    }

    const VkCommandBuffer& RawCommandBuffer::Handle() const {
        return m_Handle;
    }
    const bool RawCommandBuffer::IsSecondary() const {
        return m_Secondary;
    }
    void RawCommandBuffer::AddImageMemoryBarrier(const VkImageMemoryBarrier2& barier) {
        m_ImageMemoryBariers.push_back(barier);
    }
    void RawCommandBuffer::AddBufferMemoryBarrier(const VkBufferMemoryBarrier2& barier) {
        m_BufferMemoryBariers.push_back(barier);
    }

    void RawCommandBuffer::FlushBariers() {
        PROFILER_SCOPE("Engine::Vulkan::Managed::RawCommandBuffer::FlushBariers");

        if (!m_ImageMemoryBariers.empty() || !m_BufferMemoryBariers.empty()) {
            VkDependencyInfo info{};
            info.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            info.imageMemoryBarrierCount  = m_ImageMemoryBariers.size();
            info.pImageMemoryBarriers     = m_ImageMemoryBariers.data();
            info.bufferMemoryBarrierCount = m_BufferMemoryBariers.size();
            info.pBufferMemoryBarriers    = m_BufferMemoryBariers.data();

            vkCmdPipelineBarrier2(m_Handle, &info);

            m_ImageMemoryBariers.clear();
            m_BufferMemoryBariers.clear();
        }
    }
}  // namespace Engine::Vulkan::Managed