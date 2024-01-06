#include "Renderer.h"

#include "Debugger.h"
#include "Device.h"
#include "Helpers.h"
#include "Instance.h"
#include "Window.h"
#include "Queue.h"

namespace Vulkan {

    Renderer::Renderer() {
        QueuesSpecification spec = {
            {QueueFamily::Graphics,     1},
            {QueueFamily::Presentation, 1},
        };

        m_Instance          = Instance::Create();
        m_Debugger          = Debugger::Create(m_Instance);
        m_Device            = m_Instance->CreateBestDevice(spec);
        m_GraphicsQueue     = CreateRef<Queue>(m_Device->GetQueue(QueueFamily::Graphics, 0));
        m_PresentationQueue = CreateRef<Queue>(m_Device->GetQueue(QueueFamily::Presentation, 0));
    }

    Ref<Instance> Renderer::GetInstance() {
        return m_Instance;
    }

    Ref<Device> Renderer::GetDevice() {
        return m_Device;
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

    Ref<Queue> Renderer::GetGraphicsQueue() {
        return m_GraphicsQueue;
    }

    Ref<Queue> Renderer::GetPresentationQueue() {
        return m_PresentationQueue;
    }

}  // namespace Vulkan