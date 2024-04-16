#pragma once

#include "Common.h"
#include "Object.h"

namespace Engine::Vulkan {

    class SwapChain : public Object {
    public:
        SwapChain(Window* window, Ref<Device> device);
        ~SwapChain();

        const VkSwapchainKHR& Handle();
        const VkImage&        GetImage(uint32_t index);
        const VkImageView&    GetImageView(uint32_t index);

        void Resize(uint32_t width, uint32_t height);

        VkFormat   GetFormat() const;
        VkExtent2D GetExtent() const;
        uint32_t   ImageCount() const;

    private:
        bool ChangeExtent(VkExtent2D extent);
        void Rebuild();

    private:
        VkExtent2D         m_Extent = {};
        VkSurfaceFormatKHR m_Format = {};

        SwapChainSupportDetails  m_Details   = {};
        Window*                  m_Window    = nullptr;
        Ref<Device>              m_Device    = nullptr;
        VkSurfaceKHR             m_Surface   = VK_NULL_HANDLE;
        VkSwapchainKHR           m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage>     m_Images;
        std::vector<VkImageView> m_ImageViews;
    };

}  // namespace Engine::Vulkan