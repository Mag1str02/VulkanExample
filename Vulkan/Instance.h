#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"
#include "Utils/Singleton.h"

namespace Vulkan {

    class Instance : public Singleton<Instance> {
    public:
        SINGLETON(Instance);

        S_METHOD_DEF(VkInstance, Handle, ());

    private:
        VkInstance m_Handle = VK_NULL_HANDLE;
    };

}  // namespace Vulkan