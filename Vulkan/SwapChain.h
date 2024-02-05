#pragma once

#include <volk.h>

#include "Vulkan/Common.h"

class Window;

namespace Vulkan {

    class SwapChain {
    public:
        ~SwapChain();

        const VkSwapchainKHR& Handle();
        const VkImage&        GetImage(uint32_t index);
        const VkImageView&    GetImageView(uint32_t index);

        void Resize(uint32_t width, uint32_t height);

        VkFormat   GetFormat() const;
        VkExtent2D GetExtent() const;
        uint32_t   Size() const;

        NO_COPY_CONSTRUCTORS(SwapChain);
        NO_MOVE_CONSTRUCTORS(SwapChain);

    private:
        SwapChain(Ref<Window> window, Ref<Device> device);

        bool ChangeExtent(VkExtent2D extent);
        void Rebuild();

    private:
        friend class Device;

        VkExtent2D         m_Extent = {};
        VkSurfaceFormatKHR m_Format = {};

        SwapChainSupportDetails  m_Details   = {};
        Ref<Window>              m_Window    = nullptr;
        Ref<Device>              m_Device    = nullptr;
        VkSurfaceKHR             m_Surface   = VK_NULL_HANDLE;
        VkSwapchainKHR           m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage>     m_Images;
        std::vector<VkImageView> m_ImageViews;
    };

}  // namespace Vulkan