#pragma once

#include "Instance.h"

namespace Vulkan {
    class Debugger : public Singleton<Debugger> {
        SINGLETON(Debugger);

    public:
        S_METHOD_DEF(VkBool32, OnMessage, (const std::string& msg));

    private:
        VkDebugUtilsMessengerEXT m_DebugMessanger = VK_NULL_HANDLE;
    };
    VkDebugUtilsMessengerCreateInfoEXT GenDebuggerCreateInfo();
}  // namespace Vulkan