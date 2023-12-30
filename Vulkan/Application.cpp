#include "Application.h"

#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "Assert.h"

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
    VkResult DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
            return VK_SUCCESS;
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static const std::vector<const char*> s_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    std::vector<const char*> GetRequiredLayers() {
        return s_ValidationLayers;
    }

    std::vector<const char*> GetRequiredExtensions() {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (DE_VK_ENABLE_VALIDATION_LAYER) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    std::unordered_set<std::string> FindUnsupportedLayers() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        std::unordered_set<std::string> supportedLayers, unsupportedLayers;

        for (const auto& layerProperties : availableLayers) {
            supportedLayers.insert(layerProperties.layerName);
        }

        auto requitredLayers = GetRequiredLayers();
        for (const auto& layerName : requitredLayers) {
            if (!supportedLayers.contains(layerName)) {
                unsupportedLayers.insert(layerName);
            }
        }

        return unsupportedLayers;
    }

    std::unordered_set<std::string> FindUnsupportedExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::unordered_set<std::string> supportedExtensions, unsupportedExtensions;
        for (const auto& ext : extensions) {
            supportedExtensions.insert(ext.extensionName);
        }
        auto requitredExtensions = GetRequiredExtensions();

        for (const auto& ext : requitredExtensions) {
            if (!supportedExtensions.contains(ext)) {
                unsupportedExtensions.insert(ext);
            }
        }
        return unsupportedExtensions;
    }

    void CheckExtensionSupport() {
        auto unsupportedExtensions = FindUnsupportedExtensions();
        if (!unsupportedExtensions.empty()) {
            for (const auto& ext : unsupportedExtensions) {
                std::cerr << "Found unsupported extension: " << ext << std::endl;
            }
            DE_ASSERT(false, std::format("Found {} unsupported extensions", unsupportedExtensions.size()));
        }
    }

    void CheckLayersSupport() {
        auto unsupportedLayers = FindUnsupportedLayers();
        if (!unsupportedLayers.empty()) {
            for (const auto& unsupportedLayer : unsupportedLayers) {
                std::cerr << "Found unsupported layer: " << unsupportedLayer << std::endl;
            }
            DE_ASSERT(false, std::format("Found {} unsupported layers", unsupportedLayers.size()));
        }
    }
    static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugMessageHandler(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                                                VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                void*                                       pUserData) {
        std::cerr << "VK: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VKDebugMessageHandler;
        return createInfo;
    }

}  // namespace

Application::Application() {
    InitGLFW();
    InitVulkan();
    OnStartUp();
}
Application::~Application() {
    OnShutDown();
    TerminateVulkan();
    TerminateGLFW();
}
void Application::Run() {
    while (!glfwWindowShouldClose(m_Window)) {
        Loop();
    }
}

void Application::Loop() {
    glfwPollEvents();
    OnLoop();
}

void Application::InitGLFW() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    DE_ASSERT(m_Window, "Failed to create Vulkan Window");
}
void Application::TerminateGLFW() {
    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void Application::InitVulkan() {
    // Check prerequirements
    {
        CheckExtensionSupport();
        CheckLayersSupport();
    }

    // Create VkInstance
    {
        auto requitredLayers     = GetRequiredLayers();
        auto requitredExtensions = GetRequiredExtensions();
        auto debugCreateInfo     = DebugMessengerCreateInfo();

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
        createInfo.enabledLayerCount       = requitredLayers.size();
        createInfo.ppEnabledLayerNames     = requitredLayers.data();
        createInfo.enabledExtensionCount   = requitredExtensions.size();
        createInfo.ppEnabledExtensionNames = requitredExtensions.data();

        if (DE_VK_ENABLE_VALIDATION_LAYER) {
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }

        auto result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
        DE_ASSERT(result == VK_SUCCESS, "Failed to create instance");

        if (DE_VK_ENABLE_VALIDATION_LAYER) {
            auto result = CreateDebugUtilsMessengerEXT(m_VkInstance, &debugCreateInfo, nullptr, &m_DebugMessanger);
            DE_ASSERT(result == VK_SUCCESS, "Failed to create debug handler");
        }
    }
}
void Application::TerminateVulkan() {
    if (DE_VK_ENABLE_VALIDATION_LAYER) {
        DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessanger, nullptr);
    }
    vkDestroyInstance(m_VkInstance, nullptr);
}