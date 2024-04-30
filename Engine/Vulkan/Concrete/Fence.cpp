#include "Fence.h"

#include "Engine/Vulkan/Device.h"

namespace Engine::Vulkan::Concrete {
    Fence::Fence(Ref<Device> device) {
        VkFence           fence;
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        VK_CHECK(vkCreateFence(device->GetLogicDevice(), &info, nullptr, &fence));

        Init(device, fence);
    }
    Fence::~Fence() {
        vkDestroyFence(m_Device->GetLogicDevice(), m_Handle, nullptr);
    }

}  // namespace Engine::Vulkan::Concrete