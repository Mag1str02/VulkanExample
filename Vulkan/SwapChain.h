#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"
#include "Helpers.h"

class Window;

namespace Vulkan {

    class SwapChain {
    public:
        static Ref<SwapChain> Create(Ref<Window> window, Ref<Renderer> renderer);
        ~SwapChain();

        NO_COPY_CONSTRUCTORS(SwapChain);
        NO_MOVE_CONSTRUCTORS(SwapChain);

        VkSwapchainKHR Handle();
        VkImage        GetImage(uint32_t index);
        VkImageView    GetImageView(uint32_t index);
        void           Resize(uint32_t width, uint32_t height);

        VkFormat   GetFormat() const;
        VkExtent2D GetExtent() const;
        uint32_t   Size() const;

    private:
        SwapChain(Ref<Window> window, Ref<Renderer> renderer);

        bool ChangeExtent(VkExtent2D extent);
        void Rebuild();

        VkExtent2D         m_Extent = {};
        VkSurfaceFormatKHR m_Format = {};

        SwapChainSupportDetails  m_Details   = {};
        Ref<Window>              m_Window    = nullptr;
        Ref<Renderer>            m_Renderer  = nullptr;
        VkSurfaceKHR             m_Surface   = VK_NULL_HANDLE;
        VkSwapchainKHR           m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage>     m_Images;
        std::vector<VkImageView> m_ImageViews;
    };

}  // namespace Vulkan