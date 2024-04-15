#include "Debugger.h"

namespace Engine::Vulkan {
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

    static std::unordered_map<Instance*, Debugger*>& GetDebuggerMapping() {
        static std::unordered_map<Instance*, Debugger*> mapping;
        return mapping;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugMessageHandler(VkDebugUtilsMessageSeverityFlagBitsEXT,
                                                                VkDebugUtilsMessageTypeFlagsEXT,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                void*                                       pUserData) {
        const auto& mapping = GetDebuggerMapping();
        auto        it      = mapping.find(reinterpret_cast<Instance*>(pUserData));
        if (it == mapping.end()) {
            std::cerr << "VK: " << pCallbackData->pMessage << std::endl;
            return VK_FALSE;
        }
        return it->second->OnMessage(pCallbackData->pMessage);
    }

    VkDebugUtilsMessengerCreateInfoEXT Debugger::GenCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VKDebugMessageHandler;
        return createInfo;
    }

    Debugger::Debugger(Ref<Instance> instance) {
        DE_ASSERT(instance, "Cannot create debugger with nullptr instance");
        m_Instance = instance;

        auto& mapping = GetDebuggerMapping();
        DE_ASSERT(!mapping.contains(m_Instance.get()), "Cannot create 2 debuggers for same instance");
        mapping.emplace(m_Instance.get(), this);

#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        auto debugCreateInfo      = GenCreateInfo();
        debugCreateInfo.pUserData = m_Instance.get();
        auto result               = CreateDebugUtilsMessengerEXT(m_Instance->Handle(), &debugCreateInfo, nullptr, &m_Handle);
        DE_ASSERT(result == VK_SUCCESS, "Failed to create debug handler");
#endif
    }

    Debugger::~Debugger() {
#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        DestroyDebugUtilsMessengerEXT(m_Instance->Handle(), m_Handle, nullptr);
#endif

        auto& mapping = GetDebuggerMapping();
        DE_ASSERT(mapping.contains(m_Instance.get()), "Internal mapping error");
        mapping.erase(m_Instance.get());
    }

    VkBool32 Debugger::OnMessage(const std::string& msg) {
        std::cerr << "Vulkan: " << msg << std::endl << std::endl;
        return VK_FALSE;
    }
}  // namespace Vulkan
