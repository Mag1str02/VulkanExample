#include "Application.h"

#include <iostream>
#include <vector>

#include "Assert.h"

void ListAvailableExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "Available extensions:\n" << std::endl;

    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << std::endl;
    }
}

Application::Application() {
    InitGLFW();
    InitVulkan();
    ListAvailableExtensions();
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
    // Create VkInstance
    {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

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
        createInfo.enabledExtensionCount   = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount       = 0;

        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
        DE_ASSERT(result == VK_SUCCESS, "Failed to create instance");
    }
}
void Application::TerminateVulkan() {
    vkDestroyInstance(m_VkInstance, nullptr);
}