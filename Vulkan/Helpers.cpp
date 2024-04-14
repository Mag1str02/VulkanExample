#include "Helpers.h"

#include <GLFW/glfw3.h>
#include <volk.h>

namespace Vulkan {
    std::unordered_set<uint32_t> QueueFamilyIndices::GetUniqueIndicies() const {
        std::unordered_set<uint32_t> res;
        for (const auto& [_, i] : m_Families) {
            res.insert(i);
        }
        return res;
    }
    std::optional<uint32_t> QueueFamilyIndices::GetFamilyIndex(QueueFamily family) const {
        if (auto it = m_Families.find(family); it != m_Families.end()) {
            return it->second;
        }
        return {};
    }

    void QueueFamilyIndices::AddMapping(QueueFamily family, uint32_t index) {
        m_Families.emplace(family, index);
    }

}  // namespace Vulkan

namespace Vulkan::Helpers {
    static const std::vector<const char*> s_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };
    static const std::vector<const char*> s_DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
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

        std::unordered_set<std::string> FindUnsupportedDeviceExtension(VkPhysicalDevice device) {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            auto                            requiredExtensions = GetRequiredDeviceExtensions();
            std::unordered_set<std::string> unsupportedExtensions(requiredExtensions.begin(), requiredExtensions.end());
            for (const auto& extension : availableExtensions) {
                unsupportedExtensions.erase(extension.extensionName);
            }

            return unsupportedExtensions;
        }

        VkPhysicalDeviceProperties GetDeviceProperties(VkPhysicalDevice device) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            return deviceProperties;
        }

    }  // namespace

    QueueFamilyIndices GetDeviceQueueFamilies(VkInstance instance, VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
            const auto& props = queueFamilies[i];
            if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.AddMapping(QueueFamily::Graphics, i);
            }
            if (glfwGetPhysicalDevicePresentationSupport(instance, device, i)) {
                indices.AddMapping(QueueFamily::Presentation, i);
            }
        }

        return indices;
    }

    std::vector<const char*> GetRequiredLayers() {
        return s_ValidationLayers;
    }

    std::vector<const char*> GetRequiredInstanceExtensions() {
        uint32_t     glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        extensions.insert(extensions.end(), s_InstanceExtensions.begin(), s_InstanceExtensions.end());

#if DE_VK_ENABLE_VALIDATION_LAYER == 1
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }

    std::vector<const char*> GetRequiredDeviceExtensions() {
        return s_DeviceExtensions;
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

    bool CheckDevice(VkInstance instance, VkPhysicalDevice device, const QueuesSpecification& specification) {
        {
            auto deviceProperties = GetDeviceProperties(device);
            if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                return false;
            }
        }
        {
            auto queueFamilies = GetDeviceQueueFamilies(instance, device);
            for (const auto& [family, amount] : specification) {
                if (amount != 0 && !queueFamilies.GetFamilyIndex(family)) {
                    return false;
                }
            }
        }
        {
            auto unsupportedExtensions = FindUnsupportedDeviceExtension(device);
            if (!unsupportedExtensions.empty()) {
                return false;
            }
        }
        return true;
    }

    SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.m_Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        details.m_SurfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.m_SurfaceFormats.data());

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        details.m_PresentationModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.m_PresentationModes.data());

        return details;
    }

    void InsertImageMemoryBarier(VkCommandBuffer         buffer,
                                 VkImage                 image,
                                 VkImageLayout           oldLayout,
                                 VkImageLayout           newLayout,
                                 VkPipelineStageFlagBits srcStage,
                                 VkPipelineStageFlagBits dstStage) {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        imageMemoryBarrier.oldLayout        = oldLayout;
        imageMemoryBarrier.newLayout        = newLayout;
        imageMemoryBarrier.image            = image;
        imageMemoryBarrier.subresourceRange = {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };

        vkCmdPipelineBarrier(buffer,
                             srcStage,  // srcStageMask
                             dstStage,  // dstStageMask
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,                   // imageMemoryBarrierCount
                             &imageMemoryBarrier  // pImageMemoryBarriers
        );
    }

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        DE_ASSERT(false, "Failed to find suitable memory type!");
        return 0;
    }
}  // namespace Vulkan::Helpers