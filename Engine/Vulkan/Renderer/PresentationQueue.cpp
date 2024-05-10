#include "PresentationQueue.h"

#include "Device.h"

namespace Engine::Vulkan {

    PresentationQueue::PresentationQueue(Device* device, VkQueue queue, uint32_t queue_family_index) : Queue(device, queue, queue_family_index) {}

}  // namespace Engine::Vulkan