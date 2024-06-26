#pragma once

#include "Config.h"

namespace Engine::Vulkan {

    class Instance : public NonCopyMoveable {
    public:
        static Ref<Instance> Create(const Config& config);

        ~Instance();

        VkInstance                    Handle();
        std::vector<VkPhysicalDevice> GetAllDevices();
        uint32_t                      GetDeviceCount();

    private:
        Instance(const Config& config);

    private:
        VkInstance m_Handle = VK_NULL_HANDLE;
    };

}  // namespace Engine::Vulkan