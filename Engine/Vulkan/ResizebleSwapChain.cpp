#include "ResizebleSwapChain.h"

#include "Device.h"
#include "Window.h"

namespace Engine::Vulkan {

    Ref<ResizebleSwapChain> ResizebleSwapChain::Create(Window* window, Ref<Device> device) {
        return Ref<ResizebleSwapChain>(new ResizebleSwapChain(window, device));
    }
    ResizebleSwapChain::ResizebleSwapChain(Window* window, Ref<Device> device) : m_Device(device), m_Window(window) {
        m_SwapChain = SwapChain::Create(m_Window->GetSurface(), m_Device, m_Window->GetExtent());
    }

    Ref<Task> ResizebleSwapChain::CreateAquireImageTask() {
        return CreateRef<ResizebleSwapChain::PresentAquireTask>(shared_from_this());
    }
    Ref<IImage> ResizebleSwapChain::GetCurrentImage() {
        return m_SwapChain->GetCurrentImage();
    }

    VkExtent2D ResizebleSwapChain::GetExtent() const {
        return m_SwapChain->GetExtent();
    }
    VkFormat ResizebleSwapChain::GetFormat() const {
        return m_SwapChain->GetFormat();
    }

    void ResizebleSwapChain::PresentAquire(VkQueue queue, Fence& fence) {
        m_SwapChain->PresentLatest(queue);
        while (true) {
            VkResult res = vkAcquireNextImageKHR(m_Device->GetLogicDevice(),
                                                 m_SwapChain->Handle(),
                                                 UINT64_MAX,
                                                 VK_NULL_HANDLE,
                                                 fence.Handle(),
                                                 &m_SwapChain->m_LatestImage);
            if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR) {
                m_SwapChain = SwapChain::Create(m_Window->GetSurface(), m_Device, m_Window->GetExtent(), m_SwapChain->Handle());
            } else {
                break;
            }
        }
    }

    ResizebleSwapChain::PresentAquireTask::PresentAquireTask(Ref<ResizebleSwapChain> swapchain) :
        m_AquiredFence(swapchain->m_Device), m_SwapChain(swapchain) {}

    void ResizebleSwapChain::PresentAquireTask::Run(VkQueue queue) {
        m_SwapChain->PresentAquire(queue, m_AquiredFence);
    }
    bool ResizebleSwapChain::PresentAquireTask::IsCompleted() {
        return m_AquiredFence.IsSignaled();
    }

}  // namespace Engine::Vulkan