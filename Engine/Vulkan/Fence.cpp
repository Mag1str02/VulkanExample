#include "Fence.h"

#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan {
    Fence::Fence(Ref<Device> device) : m_Device(device) {
        VkFence           fence;
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        VK_CHECK(vkCreateFence(device->GetLogicDevice(), &info, nullptr, &fence));

        Init(device->GetLogicDevice(), fence);
    }
    Fence::~Fence() {
        vkDestroyFence(m_Device->GetLogicDevice(), m_Handle, nullptr);
    }

}  // namespace Engine::Vulkan