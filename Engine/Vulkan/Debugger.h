#pragma once

#include "Instance.h"

namespace Engine::Vulkan {

    class Debugger : public HandledStorage {
    public:
        Debugger(Instance* instance);
        ~Debugger();

        static VkDebugUtilsMessengerCreateInfoEXT GenCreateInfo();

        VkBool32 OnMessage(const std::string& msg);

    private:
        VkDebugUtilsMessengerEXT m_Handle   = VK_NULL_HANDLE;
        Instance*                m_Instance = nullptr;
    };

}  // namespace Engine::Vulkan