#include "Config.h"

#include <GLFW/glfw3.h>

namespace Vulkan::Config {
    static const std::vector<const char*> s_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };
    static const std::vector<const char*> s_DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    static const std::vector<const char*> s_InstanceExtensions = {};

    namespace {
        std::unordered_set<std::string> FindUnsupportedLayers() {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            auto                            requitredLayers = GetRequiredLayers();
            std::unordered_set<std::string> unsupportedLayers(requitredLayers.begin(), requitredLayers.end());

            for (const auto& layerProperties : availableLayers) {
                unsupportedLayers.erase(layerProperties.layerName);
            }

            return unsupportedLayers;
        }

        std::unordered_set<std::string> FindUnsupportedExtensions() {
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

            auto                            requitredExtensions = GetRequiredInstanceExtensions();
            std::unordered_set<std::string> unsupportedExtensions(requitredExtensions.begin(), requitredExtensions.end());
            for (const auto& ext : extensions) {
                unsupportedExtensions.erase(ext.extensionName);
            }

            return unsupportedExtensions;
        }
    }  // namespace

    std::vector<const char*> GetRequiredLayers() {
        return s_ValidationLayers;
    }

    std::vector<const char*> GetRequiredInstanceExtensions() {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.insert(extensions.end(), s_InstanceExtensions.begin(), s_InstanceExtensions.end());

        if (DE_VK_ENABLE_VALIDATION_LAYER) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void CheckInstanceExtensionSupport() {
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
}  // namespace Vulkan::Config