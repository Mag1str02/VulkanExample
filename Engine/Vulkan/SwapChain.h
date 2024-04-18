#pragma once

#include "Common.h"
#include "Object.h"

namespace Engine::Vulkan {

    class SwapChain : public Object {
    public:
        SwapChain(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size);
        ~SwapChain();

        VkSwapchainKHR Handle();
        VkImage        GetImage(uint32_t index);
        VkImageView    GetImageView(uint32_t index);

        void Resize(uint32_t width, uint32_t height);

        VkFormat   GetFormat() const;
        VkExtent2D GetExtent() const;
        uint32_t   ImageCount() const;

    private:
        void GetSwapChainSupportDetails();
        bool ChangeExtent(VkExtent2D extent);
        void Rebuild();
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        m_Capabilities;
            std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
            std::vector<VkPresentModeKHR>   m_PresentationModes;
        };

    private:
        VkExtent2D         m_Extent = {};
        VkSurfaceFormatKHR m_Format = {};

        SwapChainSupportDetails m_Details = {};

        Ref<Device>              m_Device;
        VkSurfaceKHR             m_Surface   = VK_NULL_HANDLE;
        VkSwapchainKHR           m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage>     m_Images;
        std::vector<VkImageView> m_ImageViews;
    };

}  // namespace Engine::Vulkan