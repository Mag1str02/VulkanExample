#pragma once

#include "Instance.h"

namespace Engine::Vulkan {

    class Debugger {
    public:
        static VkDebugUtilsMessengerCreateInfoEXT GenCreateInfo();

        ~Debugger();

        VkBool32 OnMessage(const std::string& msg);

        NO_COPY_CONSTRUCTORS(Debugger);
        NO_MOVE_CONSTRUCTORS(Debugger);

    private:
        Debugger(Ref<Instance> instance);

    private:
        friend class Instance;

        VkDebugUtilsMessengerEXT m_Handle   = VK_NULL_HANDLE;
        Ref<Instance>            m_Instance = nullptr;
    };

}  // namespace Vulkan