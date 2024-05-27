#include "Fence.h"

#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan::Managed {

    Fence::Fence(VkDevice device, VkFence fence) {
        Init(device, fence);
    }
    void Fence::Init(VkDevice device, VkFence fence) {
        m_Device = device;
        m_Handle = fence;
    }

    const VkFence& Fence::Handle() const {
        return m_Handle;
    }
    bool Fence::IsSignaled() const {
        auto status = vkGetFenceStatus(m_Device, m_Handle);
        switch (status) {
            case VK_SUCCESS: return true;
            case VK_NOT_READY: return false;
            default: DE_ASSERT_FAIL("Bad fence wait result {}", (int64_t)status);
        }
    }

    void Fence::Wait() const {
        PROFILER_SCOPE("Engine::Vulkan::Managed::Fence::Wait");
        VK_CHECK(vkWaitForFences(m_Device, 1, &m_Handle, VK_TRUE, UINT64_MAX));
    }
    void Fence::Reset() {
        VK_CHECK(vkResetFences(m_Device, 1, &m_Handle));
    }

}  // namespace Engine::Vulkan::Managed