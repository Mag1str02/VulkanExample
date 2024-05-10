#include "Renderer.h"

#include "Debugger.h"
#include "Device.h"
#include "Instance.h"

namespace Engine::Vulkan {

    Renderer::Renderer() {
        InitConfig();

        m_Instance                   = Instance::Create(m_Config);
        m_Debugger                   = Debugger::Create(m_Instance);
        VkPhysicalDevice best_device = m_Config.ChooseDevice(m_Instance->Handle(), m_Instance->GetAllDevices());

        m_Device = Device::Create(best_device, m_Instance, m_Config);
        m_Executor.Construct(m_Device);
    }
    Renderer::~Renderer() {
        m_Executor.Destruct();
        m_Debugger.reset();
        m_Device.reset();

        DE_ASSERT(m_Instance.use_count() == 1, "This reference should be the last one");
        m_Instance.reset();
    }

    void Renderer::InitConfig() {
        m_Config.AddLayer("VK_LAYER_KHRONOS_validation");

        m_Config.AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME);
        m_Config.AddDeviceExtension(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME);
    }

    void Renderer::Submit(Ref<Task> task) {
        m_Executor->Submit(task);
    }

    Ref<Device> Renderer::GetDevice() {
        return m_Device;
    }

    Ref<Instance> Renderer::GetInstance() {
        return m_Instance;
    }

}  // namespace Engine::Vulkan