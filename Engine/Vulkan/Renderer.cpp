#include "Renderer.h"

#include "Debugger.h"
#include "Device.h"
#include "Helpers.h"
#include "Instance.h"
#include "Queue.h"
#include "Window.h"

namespace Engine::Vulkan {

    Renderer::Renderer() {
        QueuesSpecification spec = {
            {QueueFamily::Graphics, 1},
 // {QueueFamily::Presentation, 1},
        };

        m_Instance      = Instance::Create();
        m_Debugger      = m_Instance->CreateDebugger();
        m_Device        = m_Instance->CreateBestDevice(spec);
        m_GraphicsQueue = m_Device->GetQueue(QueueFamily::Graphics, 0);

        // TODO: Ensuer that m_GraphicsQueue is universal queue (support graphics, compute and presentation)
        m_PresentationQueue = m_GraphicsQueue;
        // m_PresentationQueue = m_Device->GetQueue(QueueFamily::Presentation, 0);
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