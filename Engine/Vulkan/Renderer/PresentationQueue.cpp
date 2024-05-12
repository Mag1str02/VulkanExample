#include "PresentationQueue.h"

#include "Device.h"

namespace Engine::Vulkan {

    PresentationQueue::PresentationQueue(Device* device, VkQueue queue, uint32_t queue_family_index) : Queue(device, queue, queue_family_index) {}
    VkResult PresentationQueue::Present(Ref<SwapChain::Image> image, VkSemaphore wait_semaphore) {
        VkResult res;
        uint32_t index = image->GetIndex();

        VkPresentInfoKHR info{};
        info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.swapchainCount     = 1;
        info.pSwapchains        = &image->GetSwapChainHandle();
        info.pImageIndices      = &index;
        info.pResults           = &res;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores    = &wait_semaphore;

        VK_CHECK(vkQueuePresentKHR(m_Queue, &info));
        return res;
    }

}  // namespace Engine::Vulkan