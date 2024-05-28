#pragma once

#include "Device.h"

namespace Engine::Vulkan {
    class Executor final : NonCopyMoveable {
    public:
        Executor(Ref<Device> device);
        ~Executor();

        void    Submit(Ref<ITask> task);
        Device* GetDevice();

        Ref<IRawCommandBuffer> CreatePatchCommandBuffer();
        Ref<IRawCommandBuffer> GeneratePatchCommandBuffer(const Synchronization::Tracker& tracker);

        std::optional<VkImageMemoryBarrier2> UpdateImageState(IImage& image, const Synchronization::ImageTracker& tracker);

    private:
        void RemoveCompleted();

    private:
        Ref<CommandPool>        m_PatchCommandPool;
        Ref<Device>             m_Device;
        std::vector<Ref<ITask>> m_RunningTasks;
    };
}  // namespace Engine::Vulkan