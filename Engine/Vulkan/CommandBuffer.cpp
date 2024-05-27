#include "CommandBuffer.h"

#include "CommandBuffer.h"

#include "Engine/Vulkan/ImageView.h"
#include "Engine/Vulkan/Synchronization/AccessScope.h"

namespace Engine::Vulkan {

    Ref<CommandBuffer> CommandBuffer::Create(Ref<CommandPool> pool) {
        return Ref<CommandBuffer>(new CommandBuffer(std::move(pool)));
    }
    CommandBuffer::CommandBuffer(Ref<CommandPool> pool) : m_Pool(std::move(pool)) {
        m_MainCommandBuffer    = RawCommandBuffer::Create(m_Pool);
        m_CurrentCommandBuffer = nullptr;
    }

    void CommandBuffer::Begin() {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::Begin");
        DE_ASSERT(m_CurrentCommandBuffer == nullptr, "Command Buffer was already begun");

        m_Tracker.Reset();
        m_SecondaryCommandBuffers.clear();
        m_MainCommandBuffer->Begin();

        m_CurrentCommandBuffer = m_MainCommandBuffer.get();
    }
    void CommandBuffer::End() {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::End");
        DE_ASSERT(m_CurrentCommandBuffer == m_MainCommandBuffer.get(), "Command Buffer was not yet begun or rendering was not ended");

        m_MainCommandBuffer->End();
        m_CurrentCommandBuffer = nullptr;
    }

    void CommandBuffer::BeginRendering(const std::vector<Ref<ImageView>>& color_attachments) {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::BeginRendering");
        DE_ASSERT(m_CurrentCommandBuffer == m_MainCommandBuffer.get(), "Command Buffer was not yet begun or rendering was not ended");

        for (auto& image_view : color_attachments) {
            PrepareColorAttachment(image_view->GetImage());
        }

        Ref<RawCommandBuffer> secondary_command_buffer = RawCommandBuffer::Create(m_Pool, true);

        secondary_command_buffer->Begin();
        secondary_command_buffer->BeginRendering(color_attachments);

        m_CurrentCommandBuffer = secondary_command_buffer.get();
        m_SecondaryCommandBuffers.emplace_back(std::move(secondary_command_buffer));
    }
    void CommandBuffer::EndRendering() {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::EndRendering");
        DE_ASSERT(m_CurrentCommandBuffer == m_SecondaryCommandBuffers.back().get(), "Rendering was not started");

        m_SecondaryCommandBuffers.back()->EndRendering();
        m_SecondaryCommandBuffers.back()->End();

        m_MainCommandBuffer->ExecuteCommandBuffer(*m_SecondaryCommandBuffers.back());
        m_CurrentCommandBuffer = m_MainCommandBuffer.get();
    }
    void CommandBuffer::ClearImage(Ref<IImage> image, Vec4 clear_color) {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::ClearImage");
        DE_ASSERT(m_CurrentCommandBuffer == m_MainCommandBuffer.get(), "Command Buffer was not yet begun or rendering was not ended");

        IImage& pimage = *image;
        PrepareClear(std::move(image));
        m_MainCommandBuffer->ClearImage(pimage, clear_color);
    }

    void CommandBuffer::PreparePresent(Ref<IImage> image) {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::PreparePresent");

        RequestImageAccess(std::move(image), {VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE}, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }
    void CommandBuffer::PrepareColorAttachment(Ref<IImage> image) {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::PrepareColorAttachment");

        RequestImageAccess(std::move(image),
                           {VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT},
                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
    void CommandBuffer::PrepareClear(Ref<IImage> image) {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::PrepareClear");

        RequestImageAccess(std::move(image), {VK_PIPELINE_STAGE_2_CLEAR_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT}, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    }

    void CommandBuffer::RequestImageAccess(Ref<IImage> image, Synchronization::AccessScope scope, VkImageLayout layout) {
        PROFILER_SCOPE("Engine::Vulkan::CommandBuffer::RequestImageAccess");

        auto barrier = m_Tracker.RequestAccess(std::move(image), scope, layout);
        if (barrier) {
            m_MainCommandBuffer->AddImageMemoryBarrier(*barrier);
        }
    }

}  // namespace Engine::Vulkan