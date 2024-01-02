#include "Debugger.h"

SINGLETON_BASE(Vulkan::Debugger);

namespace Vulkan {
    namespace {
        VkResult CreateDebugUtilsMessengerEXT(VkInstance                                instance,
                                              const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                              const VkAllocationCallbacks*              pAllocator,
                                              VkDebugUtilsMessengerEXT*                 pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }
        VkResult DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                               VkDebugUtilsMessengerEXT     debugMessenger,
                                               const VkAllocationCallbacks* pAllocator) {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr) {
                func(instance, debugMessenger, pAllocator);
                return VK_SUCCESS;
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }
    }  // namespace

    static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugMessageHandler(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                                VkDebugUtilsMessageTypeFlagsEXT,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                void*) {
        if (!Debugger::Initialized()) {
            std::cerr << "VK: " << pCallbackData->pMessage << std::endl;
            return VK_FALSE;
        }
        return Debugger::OnMessage(pCallbackData->pMessage);
    }

    VkDebugUtilsMessengerCreateInfoEXT GenDebuggerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VKDebugMessageHandler;
        return createInfo;
    }

    S_INITIALIZE() {
        DE_ASSERT(Instance::Initialized(), "Cannot initialize vulkan debugger before instance");

#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        auto debugCreateInfo = GenDebuggerCreateInfo();
        auto result          = CreateDebugUtilsMessengerEXT(Instance::Handle(), &debugCreateInfo, nullptr, &m_DebugMessanger);
        DE_ASSERT(result == VK_SUCCESS, "Failed to create debug handler");
#endif
        return Void();
    }

    S_TERMINATE() {
#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        DestroyDebugUtilsMessengerEXT(Instance::Handle(), m_DebugMessanger, nullptr);
#endif
        return Void();
    }

    S_METHOD_IMPL(VkBool32, OnMessage, (const std::string& msg), (msg)) {
        std::cerr << "Vulkan: " << msg << std::endl;
        return VK_FALSE;
    }
}  // namespace Vulkan
