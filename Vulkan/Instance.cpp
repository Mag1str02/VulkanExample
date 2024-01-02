#include "Instance.h"

#include "Config.h"
#include "Debugger.h"

SINGLETON_BASE(Vulkan::Instance);

namespace Vulkan {

    S_INITIALIZE() {
        Config::CheckLayersSupport();
        Config::CheckInstanceExtensionSupport();

        auto requiredExtensions = Config::GetRequiredInstanceExtensions();
        auto requiredLayers     = Config::GetRequiredLayers();

        VkApplicationInfo appInfo{};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "No Engine";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo        = &appInfo;
        createInfo.enabledLayerCount       = requiredLayers.size();
        createInfo.ppEnabledLayerNames     = requiredLayers.data();
        createInfo.enabledExtensionCount   = requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        auto debugCreateInfo = GenDebuggerCreateInfo();
        createInfo.pNext     = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif

        auto result = vkCreateInstance(&createInfo, nullptr, &m_Handle);
        DE_ASSERT(result == VK_SUCCESS, "Failed to create instance");
        return Void();
    }
    S_TERMINATE() {
        vkDestroyInstance(m_Handle, nullptr);
        return Void();
    }
    S_METHOD_IMPL(VkInstance, Handle, (), ()) {
        return m_Handle;
    }

}  // namespace Vulkan
