#include "Fence.h"

#include "Device.h"

namespace Engine::Vulkan {

    VkFence IFence::Handle() {
        return m_Fence;
    }

    bool IFence::IsSignaled() const {
        auto status = vkGetFenceStatus(m_Device->GetLogicDevice(), m_Fence);
        DE_ASSERT(status == VK_SUCCESS || status == VK_NOT_READY, "Broken fence");
        return status == VK_SUCCESS;
    }
    void IFence::Wait() const {
        PROFILER_SCOPE("Engine::Vulkan::IFence::Wait");
        vkWaitForFences(m_Device->GetLogicDevice(), 1, &m_Fence, VK_TRUE, UINT64_MAX);
    }
    void IFence::Reset() {
        vkResetFences(m_Device->GetLogicDevice(), 1, &m_Fence);
    }

    Fence::Fence(Ref<Device> device) {
        m_Device = device;
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        vkCreateFence(m_Device->GetLogicDevice(), &info, nullptr, &m_Fence);
    }
    Fence::~Fence() {
        vkDestroyFence(m_Device->GetLogicDevice(), m_Fence, nullptr);
    }
}  // namespace Engine::Vulkan