#include "Renderer.h"

#include "Vulkan/Debugger.h"
#include "Vulkan/Device.h"
#include "Vulkan/Helpers.h"
#include "Vulkan/Instance.h"
#include "Vulkan/Queue.h"
#include "Vulkan/Window.h"

#include <backends/imgui_impl_vulkan.h>

namespace Vulkan {

    Renderer::Renderer() {
        QueuesSpecification spec = {
            {QueueFamily::Graphics,     1},
            {QueueFamily::Presentation, 1},
        };

        m_Instance          = Instance::Create();
        m_Debugger          = m_Instance->CreateDebugger();
        m_Device            = m_Instance->CreateBestDevice(spec);
        m_GraphicsQueue     = m_Device->GetQueue(QueueFamily::Graphics, 0);
        m_PresentationQueue = m_Device->GetQueue(QueueFamily::Presentation, 0);

        ImGui_ImplVulkan_LoadFunctions(
            [](const char* function_name, void* vulkan_instance) {
                return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name);
            },
            const_cast<VkInstance*>(&m_Instance->Handle()));
    }

    Ref<Instance> Renderer::GetInstance() {
        return m_Instance;
    }

    Ref<Device> Renderer::GetDevice() {
        return m_Device;
    }

    const VkDevice& Renderer::GetLogicDevice() {
        return m_Device->GetLogicDevice();
    }

    const VkPhysicalDevice& Renderer::GetPhysicalDevice() {
        return m_Device->GetPhysicalDevice();
    }

    const VkInstance& Renderer::GetInstanceHandle() {
        return m_Instance->Handle();
    }

    Ref<Queue> Renderer::GetGraphicsQueue() {
        return m_GraphicsQueue;
    }

    Ref<Queue> Renderer::GetPresentationQueue() {
        return m_PresentationQueue;
    }

}  // namespace Vulkan