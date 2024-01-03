#include "Renderer.h"

#include "Debugger.h"
#include "Device.h"
#include "Helpers.h"
#include "Instance.h"
#include "Window.h"

namespace Vulkan {

    Renderer::Renderer() {
        m_Instance = Instance::Create();
        m_Debugger = Debugger::Create(m_Instance);
        m_Device   = m_Instance->CreateBestDevice();
    }

    Ref<Instance> Renderer::GetInstance() {
        return m_Instance;
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

}  // namespace Vulkan