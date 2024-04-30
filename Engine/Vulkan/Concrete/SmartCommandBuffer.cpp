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
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::Begin");
        m_Tracker.Reset();
        m_CommandBuffer->Begin();
    }
    void SmartCommandBuffer::End() {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::End");
        m_CommandBuffer->End();
    }

    void SmartCommandBuffer::BeginRendering(const std::vector<Ref<ImageView>>& color_attachments) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::BeginRendering");
        for (auto& image_view : color_attachments) {
            PrepareColorAttachment(image_view->GetImage());
        }
        m_CommandBuffer->BeginRendering(color_attachments);
    }
    void SmartCommandBuffer::EndRendering() {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::EndRendering");
        m_CommandBuffer->EndRendering();
    }
    void SmartCommandBuffer::ClearImage(Ref<IImage> image, Vec4 clear_color) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::ClearImage");
        PrepareClear(image);
        m_CommandBuffer->ClearImage(std::move(image), clear_color);
    }

    void SmartCommandBuffer::AddImageMemoryBarrier(const VkImageMemoryBarrier2& barrier) {
        m_CommandBuffer->AddImageMemoryBarrier({barrier});
    }

    VkCommandBuffer SmartCommandBuffer::Handle() {
        return m_CommandBuffer->Handle();
    }
    void SmartCommandBuffer::PreparePresent(Ref<IImage> image) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::PreparePresent");
        RequestImageAccess(image, {VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE}, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }
    void SmartCommandBuffer::PrepareColorAttachment(Ref<IImage> image) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::PrepareColorAttachment");
        RequestImageAccess(image,
                           {VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT},
                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
    void SmartCommandBuffer::PrepareClear(Ref<IImage> image) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::PrepareClear");
        RequestImageAccess(image, {VK_PIPELINE_STAGE_2_CLEAR_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT}, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    }

    void SmartCommandBuffer::RequestImageAccess(Ref<IImage> image, Synchronization::AccessScope scope, VkImageLayout layout) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::SmartCommandBuffer::RequestImageAccess");

        auto barrier = m_Tracker.RequestAccess(std::move(image), scope, layout);
        if (barrier) {
            m_CommandBuffer->AddImageMemoryBarrier(*barrier);
        }
    }

}  // namespace Engine::Vulkan::Concrete