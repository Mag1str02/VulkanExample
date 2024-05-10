#include "Executor.h"

namespace Engine::Vulkan {

    Executor::Executor(Ref<Device> device) : m_Device(device) {}
    Executor::~Executor() {
        m_Device->GetGraphicsQueue()->WaitIdle();
        m_Device->GetPresentationQueue()->WaitIdle();
    }

    void Executor::Submit(Ref<Task> task) {}
}  // namespace Engine::Vulkan