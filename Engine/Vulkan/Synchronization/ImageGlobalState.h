#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan::Synchronizaation {

    struct ImageGlobalSyncState {
        Ref<ISemaphore> last_semaphore;
        Ref<ISemaphore> last_used_semaphore;

        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        
    };

};  // namespace Engine::Vulkan::Synchronizaation