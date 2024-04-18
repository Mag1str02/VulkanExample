#pragma once

#include "Common.h"
#include "Fence.h"
#include "Image.h"
#include "Object.h"
#include "Task.h"

namespace Engine::Vulkan {

    class SwapChain : public Object {
    public:
        static Ref<SwapChain> Create(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size);

        ~SwapChain();
        VkSwapchainKHR Handle();

        Ref<IImage> AquireNextImage();

        VkFormat   GetFormat() const;
        VkExtent2D GetExtent() const;

    private:
        SwapChain(VkSurfaceKHR surface, Ref<Device> device, VkExtent2D size);
        void GetSwapChainSupportDetails();

    private:
        class PresentAquireTask : public Task {
        public:
            PresentAquireTask(Ref<SwapChain> swapchain);
            virtual void Run(Ref<Queue> queue) override;
            virtual void Wait() override;
            Ref<IImage>  GetAquiredImage();

        private:
        private:
            Fence          m_AquiredFence;
            Ref<IImage>    m_AquiredImage;
            Ref<SwapChain> m_SwapChain;
        };

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR        m_Capabilities;
            std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
            std::vector<VkPresentModeKHR>   m_PresentationModes;
        };

        class Image : public IImage {
        public:
            Image(SwapChain* swapchain, VkImage image);
        };

    private:
        VkExtent2D         m_Extent = {};
        VkSurfaceFormatKHR m_Format = {};

        SwapChainSupportDetails m_Details = {};

        Ref<Device>        m_Device;
        VkSurfaceKHR       m_Surface   = VK_NULL_HANDLE;
        VkSwapchainKHR     m_SwapChain = VK_NULL_HANDLE;
        std::vector<Image> m_Images;
        uint32_t           m_LatestImage = -1;
    };

}  // namespace Engine::Vulkan