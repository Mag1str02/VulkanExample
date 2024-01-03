#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"
#include "Helpers.h"
#include "Utils/Singleton.h"

class Window;

namespace Vulkan {
    class Device;

    class Instance : public std::enable_shared_from_this<Instance> {
    public:
        static Ref<Instance> Create();
        ~Instance();

        NO_COPY_CONSTRUCTORS(Instance);
        NO_MOVE_CONSTRUCTORS(Instance);

        VkInstance  Handle();
        Ref<Device> CreateBestDevice(const QueuesSpecification& specification);

    private:
        uint32_t                      GetDeviceCount();
        std::vector<VkPhysicalDevice> GetAllDevices();
        std::vector<VkPhysicalDevice> GetCompatibleDevices(const QueuesSpecification& specification);

        Instance() = default;

        VkInstance m_Handle = VK_NULL_HANDLE;
    };

}  // namespace Vulkan