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
        createInfo.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = VKDebugMessageHandler;
        return createInfo;
    }

    VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        return deviceProperties;
    }

    struct QueueFamilyIndices {
        std::optional<uint32_t> m_GraphicsFamily;
        std::optional<uint32_t> m_PresentFamily;

        std::unordered_set<uint32_t> GetUniqueQueueIndicies() const {
            std::unordered_set<uint32_t> res;
            if (m_GraphicsFamily) {
                res.insert(*m_GraphicsFamily);
            }
            if (m_PresentFamily) {
                res.insert(*m_PresentFamily);
            }
            return res;
        }
    };

    QueueFamilyIndices GetDeviceQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
            const auto& props = queueFamilies[i];
            if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.m_GraphicsFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.m_PresentFamily = i;
            }
        }

        return indices;
    }

    bool CheckDevice(VkPhysicalDevice device, VkSurfaceKHR surface) {
        auto deviceProperties = GetDeviceProperties(device);
        auto queueFamilies    = GetDeviceQueueFamilies(device, surface);

        if (!queueFamilies.m_GraphicsFamily || !queueFamilies.m_PresentFamily) {
            return false;
        }
        if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return false;
        }

        return true;
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

    auto requitredLayers     = GetRequiredLayers();
    auto requitredExtensions = GetRequiredExtensions();
    auto debugCreateInfo     = DebugMessengerCreateInfo();

    // Create VkInstance and debugger
    {
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

    // Create surface
    {
        auto res = glfwCreateWindowSurface(m_VkInstance, m_Window, nullptr, &m_Surface);
        DE_ASSERT(res == VK_SUCCESS, "Failed to create window surface");
    }

    // Create device
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
        DE_ASSERT(deviceCount != 0, "Failed to find any GPU with vulkan support");
        std::vector<VkPhysicalDevice> devices(deviceCount), sutableDevices;
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (CheckDevice(device, m_Surface)) {
                auto props = GetDeviceProperties(device);
                std::cout << "VK: "
                          << "Found sutable GPU: " << props.deviceName << std::endl;
                sutableDevices.push_back(device);
            }
        }
        m_PhysicalDevice = sutableDevices.front();  // TODO: pick best gpu from sutable gpu
        DE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find sutable GPU");
    }
    auto queueFamilies = GetDeviceQueueFamilies(m_PhysicalDevice, m_Surface);

    // Create logic device
    {
        float priority     = 1.0f;
        auto  uniqueQueues = queueFamilies.GetUniqueQueueIndicies();

        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;
        for (const auto& queue : uniqueQueues) {
            VkDeviceQueueCreateInfo createInfo{};
            createInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = queue;
            createInfo.queueCount       = 1;
            createInfo.pQueuePriorities = &priority;
            queuesCreateInfo.push_back(createInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos    = queuesCreateInfo.data();
        createInfo.queueCreateInfoCount = uniqueQueues.size();
        createInfo.pEnabledFeatures     = &deviceFeatures;
        createInfo.enabledLayerCount    = requitredLayers.size();
        createInfo.ppEnabledLayerNames  = requitredLayers.data();

        auto res = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicDevice);
        DE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to create logic device");
        vkGetDeviceQueue(m_LogicDevice, *queueFamilies.m_GraphicsFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicDevice, *queueFamilies.m_PresentFamily, 0, &m_PresentQueue);
    }
}
void Application::TerminateVulkan() {
    vkDestroyDevice(m_LogicDevice, nullptr);
    vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);
    if (DE_VK_ENABLE_VALIDATION_LAYER) {
        DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessanger, nullptr);
    }
    vkDestroyInstance(m_VkInstance, nullptr);
}