#include "Executor.h"

#include "Engine/Vulkan/CommandBuffer.h"
#include "Engine/Vulkan/CommandPool.h"
#include "Engine/Vulkan/Helpers.h"
#include "Engine/Vulkan/Interface/Task.h"
#include "Engine/Vulkan/RawCommandBuffer.h"
#include "Engine/Vulkan/Synchronization/Tracker.h"

namespace Engine::Vulkan {

    namespace {
        bool IsUseless(const VkImageMemoryBarrier2& barrier) {
            if (barrier.srcStageMask != VK_PIPELINE_STAGE_2_NONE && barrier.dstStageMask != VK_PIPELINE_STAGE_2_NONE) {
                return false;
            }
            if (barrier.newLayout != barrier.oldLayout) {
                return false;
            }
            if (barrier.srcStageMask != VK_PIPELINE_STAGE_2_NONE && barrier.srcAccessMask != VK_ACCESS_2_NONE) {
                return false;
            }
            if (barrier.dstStageMask != VK_PIPELINE_STAGE_2_NONE && barrier.dstAccessMask != VK_ACCESS_2_NONE) {
                return false;
            }
            return true;
        }
    }  // namespace

    Executor::Executor(Ref<Device> device) : m_Device(std::move(device)) {
        m_PatchCommandPool = CommandPool::Create(m_Device->GetGraphicsQueue()->CreateRef(), true);
    }
    Executor::~Executor() {
        m_Device->WaitIdle();

        RemoveCompleted();

        DE_ASSERT(m_RunningTasks.empty(), "Some running tasks left");
    }

    void Executor::Submit(Ref<ITask> task) {
        PROFILER_SCOPE("Engine::Vulkan::Executor::Submit");

        task->Run(this);
        m_RunningTasks.emplace_back(std::move(task));

        RemoveCompleted();
    }

    Device* Executor::GetDevice() {
        return m_Device.get();
    }

    void Executor::RemoveCompleted() {
        PROFILER_SCOPE("Engine::Vulkan::Executor::RemoveCompleted");
        for (int64_t i = 0; i < m_RunningTasks.size(); ++i) {
            if (m_RunningTasks[i]->IsCompleted()) {
                m_RunningTasks.erase(m_RunningTasks.begin() + i);
                --i;
            }
        }
    }

    Ref<IRawCommandBuffer> Executor::CreatePatchCommandBuffer() {
        PROFILER_SCOPE("Engine::Vulkan::Executor::CreatePatchCommandBuffer");
        return RawCommandBuffer::Create(m_PatchCommandPool);
    }

    Ref<IRawCommandBuffer> Executor::GeneratePatchCommandBuffer(const Synchronization::Tracker& tracker) {
        PROFILER_SCOPE("Engine::Vulkan::Executor::GeneratePatchCommandBuffer");
        auto barriers = tracker.SynchronizeImages(this);
        if (barriers.empty()) {
            return nullptr;
        }
        auto cmd = CreatePatchCommandBuffer();
        cmd->Begin();
        for (const auto& barrier : barriers) {
            cmd->AddImageMemoryBarrier(barrier);
        }
        cmd->End();
        return cmd;
    }

    void Executor::RecordSynchronizationBarriers(CommandBuffer& buffer, const Synchronization::Tracker& tracker) {
        auto barriers = tracker.SynchronizeImages(this);
        for (const auto& barrier : barriers) {
            buffer.m_MainCommandBuffer->AddImageMemoryBarrier(barrier);
        }
    }

    std::optional<VkImageMemoryBarrier2> Executor::UpdateImageState(IImage& image, const Synchronization::ImageTracker& tracker) {
        auto&       image_suffix = image.GetSyncState();
        const auto& suffix       = tracker.GetSuffix();
        const auto& prefix       = tracker.GetPrefix();

        if (prefix.Empty()) {
            return std::nullopt;
        }

        bool emmit = false;

        VkImageMemoryBarrier2 barrier{};

        barrier.oldLayout = image_suffix.layout;
        barrier.newLayout = prefix.layout;

        if (prefix.layout != image_suffix.layout) {
            barrier.srcStageMask  = image_suffix.read_accesses.GetStages() | image_suffix.write.GetStages();
            barrier.srcAccessMask = image_suffix.write.GetWriteAccess();

            barrier.dstStageMask  = prefix.read_accesses.GetStages() | prefix.write.GetStages();
            barrier.dstAccessMask = prefix.read_accesses.GetAccess() | prefix.write.GetAccess();

            image_suffix = suffix;

            emmit = true;
        } else {
            //* image suffix ops to prefix write
            {
                VkImageMemoryBarrier2 barrier_copy{};

                barrier_copy.srcStageMask = image_suffix.read_accesses.GetStages() | image_suffix.write.GetStages();
                barrier_copy.dstStageMask = prefix.write.GetStages();

                if (!IsUseless(barrier_copy)) {
                    barrier.srcStageMask |= barrier_copy.srcStageMask;
                    barrier.dstStageMask |= barrier_copy.dstStageMask;

                    emmit = true;
                }
            }

            //* image suffix write to prefix operations
            {
                VkImageMemoryBarrier2 barrier_copy{};

                barrier_copy.srcAccessMask |= image_suffix.write.GetWriteAccess();
                barrier_copy.srcStageMask |= image_suffix.write.GetStages();

                for (const auto& [access, stages] : prefix.read_accesses.GetReaders()) {
                    auto [new_access, new_stages] = image_suffix.read_accesses.AddAccess({stages, access});
                    barrier_copy.dstAccessMask |= new_access;
                    barrier_copy.dstStageMask |= new_stages;
                }
                barrier_copy.dstAccessMask |= prefix.write.GetAccess();
                barrier_copy.dstStageMask |= prefix.write.GetStages();

                if (!IsUseless(barrier_copy)) {
                    barrier.srcAccessMask |= barrier_copy.srcAccessMask;
                    barrier.srcStageMask |= barrier_copy.srcStageMask;
                    barrier.dstAccessMask |= barrier_copy.dstAccessMask;
                    barrier.dstStageMask |= barrier_copy.dstStageMask;
                    emmit = true;
                }
            }

            if (suffix.HasWrite()) {
                image_suffix = suffix;
            }
        }

        if (emmit) {
            barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.image                           = image.Handle();
            barrier.subresourceRange.aspectMask     = Helpers::ImageAspectFlagsFromFormat(image.GetFormat());
            barrier.subresourceRange.baseMipLevel   = 0;
            barrier.subresourceRange.levelCount     = VK_REMAINING_MIP_LEVELS;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount     = VK_REMAINING_ARRAY_LAYERS;

            return barrier;
        }

        return std::nullopt;
    }

}  // namespace Engine::Vulkan