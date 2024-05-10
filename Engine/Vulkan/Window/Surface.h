#pragma once

#include "Engine/Vulkan/Common.h"

namespace Engine::Vulkan {
    class Surface final : NonCopyMoveable {
    public:
        static Ref<Surface> Create(VkSurfaceKHR surface, Ref<Device> device);
        ~Surface();

        Ref<Device>         GetDevice() const;
        const VkSurfaceKHR& Handle();

        VkSurfaceTransformFlagBitsKHR GetTransform() const;
        VkSurfaceFormatKHR            GetFormat() const;
        VkPresentModeKHR              GetPresentMode() const;
        VkExtent2D                    GetExtent() const;
        uint32_t                      GetMinImageCount() const;

        void UpdateCapabilities();

    private:
        Surface(VkSurfaceKHR surface, Ref<Device> device);

    private:
        VkSurfaceKHR m_Handle;
        Ref<Device>  m_Device;

        VkSurfaceCapabilitiesKHR        m_Capabilities;
        std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
        std::vector<VkPresentModeKHR>   m_PresentationModes;
    };
}  // namespace Engine::Vulkan