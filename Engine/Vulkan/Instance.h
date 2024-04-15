#pragma once

#include "Common.h"

#include "Engine/Utils/Singleton.h"

namespace Engine::Vulkan {

    class Instance : public RefCounted<Instance> {
    public:
        static Ref<Instance> Create();
        ~Instance();

        const VkInstance& Handle();

        Ref<Device>   CreateBestDevice(const QueuesSpecification& specification);
        Ref<Debugger> CreateDebugger();

        NO_COPY_CONSTRUCTORS(Instance);
        NO_MOVE_CONSTRUCTORS(Instance);

    private:
        Instance() = default;

        uint32_t                      GetDeviceCount();
        std::vector<VkPhysicalDevice> GetAllDevices();
        std::vector<VkPhysicalDevice> GetCompatibleDevices(const QueuesSpecification& specification);

    private:
        VkInstance m_Handle = VK_NULL_HANDLE;
    };

}  // namespace Engine::Vulkan