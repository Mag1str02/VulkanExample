#pragma once

#include "Instance.h"

namespace Engine::Vulkan {

    class Debugger : public Object {
    public:
        static VkDebugUtilsMessengerCreateInfoEXT GenCreateInfo();
        static Ref<Debugger>                      Create(Ref<Instance> instance);

        ~Debugger();

        VkBool32 OnMessage(const std::string& msg);

    private:
        Debugger(Ref<Instance> instance);

    private:
        VkDebugUtilsMessengerEXT m_Handle   = VK_NULL_HANDLE;
        Ref<Instance>            m_Instance = nullptr;
    };

}  // namespace Engine::Vulkan