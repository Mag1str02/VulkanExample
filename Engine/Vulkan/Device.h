#pragma once

#include "Common.h"

namespace Engine::Vulkan {

    class Device : public HandledStorage {
    public:
        Device(VkPhysicalDevice device, Instance* instance, const Config& config);
        ~Device();

        VkDevice         GetLogicDevice();
        VkPhysicalDevice GetPhysicalDevice();
        VkQueue          GetQueue();
        uint32_t         GetQueueFamilyIndex();

    private:
        VkQueue          m_Queue          = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice         m_LogicDevice    = VK_NULL_HANDLE;
        uint32_t         m_QueueFamilyIndex;

        Instance* m_Instance = nullptr;
    };

};  // namespace Engine::Vulkan