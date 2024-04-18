#include "Renderer.h"

#include "Debugger.h"
#include "Device.h"
#include "Helpers.h"
#include "Instance.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace Engine::Vulkan {

    namespace {}  // namespace

    Renderer::Renderer() {
        m_Config.AddLayer("VK_LAYER_KHRONOS_validation");
        m_Config.AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_EXT_SHADER_OBJECT_EXTENSION_NAME);

        m_Instance = CreateHandledStorage<Instance>(m_Config);
        m_Debugger = CreateHandledStorage<Debugger>(m_Instance.Get());

        VkPhysicalDevice best_device = m_Config.ChooseDevice(m_Instance->Handle(), m_Instance->GetAllDevices());
        m_Device                     = CreateHandledStorage<Device>(best_device, m_Instance.Get(), m_Config);

        m_Debugger.AddLifetimeDependency(m_Instance);
        m_Device.AddLifetimeDependency(m_Instance);
    }
    Renderer::~Renderer() {
        m_Debugger.Reset();
        m_Device.Reset();

        DE_ASSERT(m_Instance.RefCount() == 1, "This reference should be the last one");
        m_Instance.Reset();
    }

    VkDevice Renderer::GetLogicDevice() {
        return m_Device->GetLogicDevice();
    }

    VkPhysicalDevice Renderer::GetPhysicalDevice() {
        return m_Device->GetPhysicalDevice();
    }

    VkInstance Renderer::GetInstanceHandle() {
        return m_Instance->Handle();
    }

    VkQueue Renderer::GetQueue() {
        return m_Device->GetQueue();
    }
    uint32_t Renderer::GetQueueFamilyIndex() {
        return m_Device->GetQueueFamilyIndex();
    }

}  // namespace Engine::Vulkan