#include "SmartCommandBuffer.h"

#include "CommandBuffer.h"

#include "Engine/Vulkan/ImageView.h"
#include "Engine/Vulkan/Synchronization/State.h"

namespace Engine::Vulkan::Concrete {

    Ref<SmartCommandBuffer> SmartCommandBuffer::Create(Ref<CommandPool> pool) {
        return Ref<SmartCommandBuffer>(new SmartCommandBuffer(std::move(pool)));
    }
    SmartCommandBuffer::SmartCommandBuffer(Ref<CommandPool> pool) : m_CommandBuffer(CommandBuffer::Create(std::move(pool))) {}

    void SmartCommandBuffer::Begin() {
        m_Tracker.Reset();
        m_CommandBuffer->Begin();
    }
    void SmartCommandBuffer::End() {
        m_CommandBuffer->End();
    }

    void SmartCommandBuffer::BeginRendering(const std::vector<Ref<ImageView>>& color_attachments) {
        for (auto& image_view : color_attachments) {
            PrepareColorAttachment(image_view->GetImage());
        }
        m_CommandBuffer->BeginRendering(color_attachments);
    }
    void SmartCommandBuffer::EndRendering() {
        m_CommandBuffer->EndRendering();
    }
    void SmartCommandBuffer::ClearImage(Ref<IImage> image, Vec4 clear_color) {
        PrepareClear(image);
        m_CommandBuffer->ClearImage(std::move(image), clear_color);
    }

    void SmartCommandBuffer::AddImageMemoryBarrier(const VkImageMemoryBarrier2& barrier) {
        m_CommandBuffer->AddImageMemoryBarriers({barrier});
    }

    VkCommandBuffer SmartCommandBuffer::Handle() {
        return m_CommandBuffer->Handle();
    }
    void SmartCommandBuffer::PreparePresent(Ref<IImage> image) {
        RequestImageAccess(image, {VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE}, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }
    void SmartCommandBuffer::PrepareColorAttachment(Ref<IImage> image) {
        RequestImageAccess(image,
                           {VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT},
                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
    void SmartCommandBuffer::PrepareClear(Ref<IImage> image) {
        RequestImageAccess(image, {VK_PIPELINE_STAGE_2_CLEAR_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT}, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    }

    void SmartCommandBuffer::RequestImageAccess(Ref<IImage> image, Synchronization::AccessScope scope, VkImageLayout layout) {
        auto barriers = m_Tracker.RequestAccess(std::move(image), scope, layout);
        m_CommandBuffer->AddImageMemoryBarriers(barriers);
    }
    void SmartCommandBuffer::HintInitialLayout(Ref<IImage> image, VkImageLayout layout) {
        DE_ASSERT(!m_Tracker.HasImage(image), "Image was already used in buffer");
        RequestImageAccess(image, {VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE}, layout);
    }

}  // namespace Engine::Vulkan::Concrete