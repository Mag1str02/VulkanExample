#include "PresentationQueue.h"

#include "Device.h"

namespace Engine::Vulkan {

    PresentationQueue::PresentationQueue(Device* device, VkQueue queue, uint32_t queue_family_index) : Queue(device, queue, queue_family_index) {}
    VkResult PresentationQueue::Present(Ref<SwapChain::Image> image, VkSemaphore wait_semaphore, VkFence signal_fence) {
        uint32_t index = image->GetIndex();

        VkSwapchainPresentFenceInfoEXT fenceInfo{};
        fenceInfo.sType          = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_FENCE_INFO_EXT;
        fenceInfo.swapchainCount = 1;
        fenceInfo.pFences        = &signal_fence;

        VkPresentInfoKHR info{};
        info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.swapchainCount     = 1;
        info.pSwapchains        = &image->GetSwapChainHandle();
        info.pImageIndices      = &index;
        info.pResults           = nullptr;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores    = &wait_semaphore;

        if (signal_fence != VK_NULL_HANDLE) {
            info.pNext = &fenceInfo;
        }

        VkResult res = vkQueuePresentKHR(m_Queue, &info);
        DE_ASSERT(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR, "Failed to present swapcahin image");

        return res;
    }

}  // namespace Engine::Vulkan