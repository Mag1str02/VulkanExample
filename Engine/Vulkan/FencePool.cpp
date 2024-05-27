#include "FencePool.h"

#include "Engine/Vulkan/Renderer/Device.h"

namespace Engine::Vulkan {

    FencePool::Fence::Fence(Ref<FencePool> pool, VkFence handle) : Managed::Fence(pool->GetDevice()->GetLogicDevice(), handle), m_Pool(pool) {}
    FencePool::Fence::~Fence() {
        if (IsSignaled()) {
            Reset();
        }
        m_Pool->m_Fences.push_back(m_Handle);
    }

    Ref<FencePool> FencePool::Create(Ref<Device> device) {
        return Ref<FencePool>(new FencePool(device));
    }
    FencePool::~FencePool() {
        m_Device->WaitIdle();
        for (const auto& fence : m_Fences) {
            vkDestroyFence(m_Device->GetLogicDevice(), fence, nullptr);
        }
    }

    FencePool::FencePool(Ref<Device> device) : m_Device(device) {}
    Ref<IFence> FencePool::CreateFence() {
        if (m_Fences.empty()) {
            VkFence           fence;
            VkFenceCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            VK_CHECK(vkCreateFence(m_Device->GetLogicDevice(), &info, nullptr, &fence));
            m_Fences.push_back(fence);
        }
        VkFence fence = m_Fences.front();
        m_Fences.pop_front();
        return Ref<Fence>(new Fence(shared_from_this(), fence));
    }
    Ref<Device> FencePool::GetDevice() const {
        return m_Device;
    }

}  // namespace Engine::Vulkan