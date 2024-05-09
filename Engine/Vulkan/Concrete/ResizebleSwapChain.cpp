#include "ResizebleSwapChain.h"

#include "Engine/Vulkan/Device.h"
#include "Engine/Vulkan/Managed/CommandBuffer.h"
#include "Engine/Vulkan/Window.h"

namespace Engine::Vulkan::Concrete {

    Ref<ResizebleSwapChain> ResizebleSwapChain::Create(Window* window, Ref<Device> device) {
        return Ref<ResizebleSwapChain>(new ResizebleSwapChain(window, device));
    }
    ResizebleSwapChain::ResizebleSwapChain(Window* window, Ref<Device> device) : m_Device(device), m_Window(window) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::ResizebleSwapChain::ResizebleSwapChain");
        m_SwapChain = Concrete::SwapChain::Create(m_Window->GetSurface(), m_Device, m_Window->GetExtent());
    }

    Ref<Interface::SwapChain::PresentAquireTask> ResizebleSwapChain::CreateAquireImageTask() {
        return CreateRef<ResizebleSwapChain::PresentAquireTask>(shared_from_this());
    }

    VkExtent2D ResizebleSwapChain::GetExtent() const {
        return m_SwapChain->GetExtent();
    }
    VkFormat ResizebleSwapChain::GetFormat() const {
        return m_SwapChain->GetFormat();
    }

    ResizebleSwapChain::PresentAquireTask::PresentAquireTask(Ref<ResizebleSwapChain> swapchain) :
        m_Fence(swapchain->m_Device), m_SwapChain(swapchain) {
        auto latest_image = m_SwapChain->m_SwapChain->m_LatestImage;
        if (latest_image != m_SwapChain->m_SwapChain->m_Images.size()) {
            auto& image = m_SwapChain->m_SwapChain->m_Images[latest_image];
            m_PresentImageTracker.AccessRequest(Synchronization::AccessScope(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            m_PresentImageBarriers = Synchronization::ConnectSyncStates(image, image.GetSyncState(), m_PresentImageTracker.GetPrefix());
        }
    }

    void ResizebleSwapChain::PresentAquireTask::RecordBarriers(Managed::CommandBuffer& buffer) const {
        std::println("Requires {} barriers", m_PresentImageBarriers.size());
        for (const auto& barrier : m_PresentImageBarriers) {
            buffer.AddImageMemoryBarrier(barrier);
        }
    }
    bool ResizebleSwapChain::PresentAquireTask::RequiresBarriers() const {
        return !m_PresentImageBarriers.empty();
    }
    bool ResizebleSwapChain::PresentAquireTask::RequiresSemaphore() const {
        return true;
    }

    void ResizebleSwapChain::PresentAquireTask::Run(VkQueue queue, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) {
        PROFILER_SCOPE("Engine::Vulkan::Concrete::ResizebleSwapChain::PresentAquireTask::Run");

        auto res = m_SwapChain->m_SwapChain->PresentLatest(queue, wait_semaphore, m_PresentImageTracker);

        DE_ASSERT(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR, "Presentation failed");

        if (m_SwapChain->m_SubOptimalFlag) {
            RecreateSwapChain();
            m_SwapChain->m_SubOptimalFlag = false;
        }
        while (true) {
            res = vkAcquireNextImageKHR(m_SwapChain->m_Device->GetLogicDevice(),
                                        m_SwapChain->m_SwapChain->Handle(),
                                        UINT64_MAX,
                                        signal_semaphore,
                                        m_Fence.Handle(),
                                        &m_SwapChain->m_SwapChain->m_LatestImage);

            std::println("Aquired: {}", m_SwapChain->m_SwapChain->m_LatestImage);
            DE_ASSERT(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR, "Aquire failed");

            if (res == VK_ERROR_OUT_OF_DATE_KHR) {
                RecreateSwapChain();
            } else {
                if (res == VK_SUBOPTIMAL_KHR) {
                    m_SwapChain->m_SubOptimalFlag = true;
                }
                break;
            }
        };
        m_AquiredImage = Ref<IImage>(m_SwapChain->m_SwapChain, &m_SwapChain->m_SwapChain->m_Images[m_SwapChain->m_SwapChain->m_LatestImage]);
    }

    void ResizebleSwapChain::PresentAquireTask::RecreateSwapChain() {
        m_SwapChain->m_SwapChain = Concrete::SwapChain::Create(m_SwapChain->m_Window->GetSurface(),
                                                               m_SwapChain->m_Device,
                                                               m_SwapChain->m_Window->GetExtent(),
                                                               m_SwapChain->m_SwapChain->Handle());
    }

    Ref<IImage> ResizebleSwapChain::PresentAquireTask::GetAquiredImage() const {
        DE_ASSERT(m_AquiredImage != nullptr, "PresentAquireTask was not runned yet");
        return m_AquiredImage;
    }

    bool ResizebleSwapChain::PresentAquireTask::IsCompleted() const {
        return m_Fence.IsSignaled();
    }
    void ResizebleSwapChain::PresentAquireTask::Wait() const {
        m_Fence.Wait();
    }
}  // namespace Engine::Vulkan::Concrete