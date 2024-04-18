#pragma once

#include "Common.h"
#include "Config.h"

namespace Engine::Vulkan {

    class Instance : public HandledStorage {
    public:
        Instance(const Config& config);
        ~Instance();

        VkInstance                    Handle();
        std::vector<VkPhysicalDevice> GetAllDevices();
        uint32_t                      GetDeviceCount();

    private:
        VkInstance m_Handle = VK_NULL_HANDLE;
    };

}  // namespace Engine::Vulkan