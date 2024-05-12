#include "Executor.h"

namespace Engine::Vulkan {

    Executor::Executor(Ref<Device> device) : m_Device(device) {}
    Executor::~Executor() {
        m_Device->WaitIdle();
    }

    void Executor::Submit(Ref<Task> task) {
        PROFILER_SCOPE("Engine::Vulkan::Renderer::Executor::Submit");

        task->Run(this);
        m_RunningTasks.emplace_back(std::move(task));

        RemoveCompleted();
    }

    Ref<Device> Executor::GetDevice() {
        return m_Device;
    }

    void Executor::RemoveCompleted() {
        for (int64_t i = 0; i < m_RunningTasks.size(); ++i) {
            if (m_RunningTasks[i]->IsCompleted()) {
                m_RunningTasks.erase(m_RunningTasks.begin() + i);
                --i;
            }
        }
    }
}  // namespace Engine::Vulkan