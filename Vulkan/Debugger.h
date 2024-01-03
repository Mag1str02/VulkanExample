#pragma once

#include "Instance.h"

namespace Vulkan {
    class Debugger {
    public:
        NO_COPY_CONSTRUCTORS(Debugger);
        NO_MOVE_CONSTRUCTORS(Debugger);

        static Ref<Debugger>                      Create(Ref<Instance> instance);
        static VkDebugUtilsMessengerCreateInfoEXT GenCreateInfo();

        VkBool32 OnMessage(const std::string& msg);
        ~Debugger();

    private:
        Debugger() = default;

        VkDebugUtilsMessengerEXT m_Handle   = VK_NULL_HANDLE;
        Ref<Instance>            m_Instance = nullptr;
    };

}  // namespace Vulkan