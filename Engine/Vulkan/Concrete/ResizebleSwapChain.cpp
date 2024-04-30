#include "ResizebleSwapChain.h"

#include "Engine/Vulkan/Device.h"
#include "Engine/Vulkan/Window.h"

namespace Engine::Vulkan::Concrete {

    Ref<ResizebleSwapChain> ResizebleSwapChain::Create(Window* window, Ref<Device> device) {
        return Ref<ResizebleSwapChain>(new ResizebleSwapChain(window, device));
    }
    ResizebleSwapChain::ResizebleSwapChain(Window* window, Ref<Device> device) : m_Device(device), m_Window(window) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::ResizebleSwapChain::ResizebleSwapChain");
        m_SwapChain = Concrete::SwapChain::Create(m_Window->GetSurface(), m_Device, m_Window->GetExtent());
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
        PROFILER_SCOPE("Engine::Vulkan::Concrete::ResizebleSwapChain::PresentAquire");
        m_SwapChain->PresentLatest(queue);
        if (m_SubOptimalFlag) {
            m_SwapChain      = Concrete::SwapChain::Create(m_Window->GetSurface(), m_Device, m_Window->GetExtent(), m_SwapChain->Handle());
            m_SubOptimalFlag = false;
        }
        while (true) {
            VkResult res = vkAcquireNextImageKHR(m_Device->GetLogicDevice(),
                                                 m_SwapChain->Handle(),
                                                 UINT64_MAX,
                                                 VK_NULL_HANDLE,
                                                 fence.Handle(),
                                                 &m_SwapChain->m_LatestImage);
            if (res == VK_ERROR_OUT_OF_DATE_KHR) {
                m_SwapChain = Concrete::SwapChain::Create(m_Window->GetSurface(), m_Device, m_Window->GetExtent(), m_SwapChain->Handle());
            } else {
                if (res == VK_SUBOPTIMAL_KHR) {
                    m_SubOptimalFlag = true;
                }
                break;
            }
        }
    }

    ResizebleSwapChain::PresentAquireTask::PresentAquireTask(Ref<ResizebleSwapChain> swapchain) :
        m_AquiredFence(swapchain->m_Device), m_SwapChain(swapchain) {}

    void ResizebleSwapChain::PresentAquireTask::Run(VkQueue queue) {
        m_SwapChain->PresentAquire(queue, m_AquiredFence);
    }
    Ref<const IFence> ResizebleSwapChain::PresentAquireTask::GetFence() const {
        return Ref<const IFence>(shared_from_this(), &m_AquiredFence);
    }

}  // namespace Engine::Vulkan::Concrete