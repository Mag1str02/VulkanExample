#include "Surface.h"

#include "Engine/Vulkan/Renderer/Device.h"
#include "Engine/Vulkan/Renderer/Instance.h"

namespace Engine::Vulkan {

    namespace {

        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
            for (const auto& availableFormat : availableFormats) {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    return availableFormat;
                }
            }
            DE_ASSERT_FAIL("Failed to find sutable surface format");
        }

        VkPresentModeKHR ChoosePresentationMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
            for (const auto& availablePresentMode : availablePresentModes) {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    return availablePresentMode;
                }
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        uint32_t ChoseImageCount(const VkSurfaceCapabilitiesKHR& capabilities) {
            uint32_t minImageCount = capabilities.minImageCount;
            uint32_t maxImageCount = capabilities.maxImageCount;
            if (maxImageCount == 0) {
                maxImageCount = UINT32_MAX;
            }
            return std::clamp(minImageCount + 2, minImageCount, maxImageCount);
        }

    }  // namespace

    Ref<Surface> Surface::Create(VkSurfaceKHR surface, Ref<Device> device) {
        return Ref<Surface>(new Surface(surface, device));
    }
    Surface::Surface(VkSurfaceKHR surface, Ref<Device> device) : m_Handle(surface), m_Device(device) {
        UpdateCapabilities();
        uint32_t formatCount;
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice(), m_Handle, &formatCount, nullptr));
        m_SurfaceFormats.resize(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysicalDevice(), m_Handle, &formatCount, m_SurfaceFormats.data()));

        uint32_t presentModeCount;
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice(), m_Handle, &presentModeCount, nullptr));
        m_PresentationModes.resize(presentModeCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysicalDevice(), m_Handle, &presentModeCount, m_PresentationModes.data()));
    }
    Surface::~Surface() {
        vkDestroySurfaceKHR(m_Device->GetInstance()->Handle(), m_Handle, nullptr);
    }

    const VkSurfaceKHR& Surface::Handle() {
        return m_Handle;
    }

    VkSurfaceTransformFlagBitsKHR Surface::GetTransform() const {
        return m_Capabilities.currentTransform;
    }
    VkSurfaceFormatKHR Surface::GetFormat() const {
        return ChooseSurfaceFormat(m_SurfaceFormats);
    }
    VkPresentModeKHR Surface::GetPresentMode() const {
        return ChoosePresentationMode(m_PresentationModes);
    }
    VkExtent2D Surface::GetExtent() const {
        return m_Capabilities.currentExtent;
    }
    uint32_t Surface::GetMinImageCount() const {
        return ChoseImageCount(m_Capabilities);
    }

    Ref<Device> Surface::GetDevice() const {
        return m_Device;
    }

    void Surface::UpdateCapabilities() {
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device->GetPhysicalDevice(), m_Handle, &m_Capabilities));
    }

}  // namespace Engine::Vulkan