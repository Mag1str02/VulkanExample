#include "CommandBuffer.h"

#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Device.h"
#include "Engine/Vulkan/ImageView.h"
#include "Engine/Vulkan/Interface/Image.h"

namespace Engine::Vulkan::Managed {

    void CommandBuffer::Init(VkCommandBuffer handle, Ref<CommandPool> pool) {
        m_Handle = handle;
        m_Pool   = pool;

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = m_Pool->FamilyIndex();
        VK_CHECK(vkCreateCommandPool(m_Pool->GetDevice()->GetLogicDevice(), &poolInfo, nullptr, &m_SecondaryCommandPool));
    }
    CommandBuffer::~CommandBuffer() {
        vkDestroyCommandPool(m_Pool->GetDevice()->GetLogicDevice(), m_SecondaryCommandPool, nullptr);
    }

    void CommandBuffer::BeginRendering(std::vector<Ref<ImageView>> color_attachments) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = m_Pool->Handle();
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(m_Pool->GetDevice()->GetLogicDevice(), &allocInfo, &m_SecondaryCommandBuffer));

        VkCommandBufferInheritanceInfo inheritance{};
        inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = &inheritance;
        VK_CHECK(vkBeginCommandBuffer(m_SecondaryCommandBuffer, &beginInfo));

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

        vkCmdBeginRendering(m_SecondaryCommandBuffer, &info);
    }
    void CommandBuffer::EndRendering() {
        vkCmdEndRendering(m_SecondaryCommandBuffer);
        VK_CHECK(vkEndCommandBuffer(m_SecondaryCommandBuffer));

        FlushBariers();
        vkCmdExecuteCommands(m_Handle, 1, &m_SecondaryCommandBuffer);
        m_SecondaryCommandBuffer = VK_NULL_HANDLE;
    }
    void CommandBuffer::ClearImage(Ref<IImage> image, Vec4 clear_color) {
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

        vkCmdClearColorImage(m_Handle, image->Handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);
    }

    void CommandBuffer::ResetSecondaryCommandBuffers() {
        vkResetCommandPool(m_Pool->GetDevice()->GetLogicDevice(), m_SecondaryCommandPool, 0);
    }

    VkCommandBuffer CommandBuffer::Handle() {
        return m_Handle;
    }
    void CommandBuffer::AddImageMemoryBarier(VkImageMemoryBarrier2 barier) {
        m_ImageMemoryBariers.push_back(std::move(barier));
    }
    void CommandBuffer::AddBufferMemoryBarier(VkBufferMemoryBarrier2 barier) {
        m_BufferMemoryBariers.push_back(std::move(barier));
    }

    void CommandBuffer::FlushBariers() {
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
}  // namespace Engine::Vulkan::Managed