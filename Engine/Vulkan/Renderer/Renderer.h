#pragma once

#include "Config.h"
#include "Device.h"
#include "Executor.h"
#include "Task.h"

namespace Engine::Vulkan {

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        Ref<Instance> GetInstance();
        Ref<Device>   GetDevice();

        std::optional<std::string> SubmitRenderGraph(RenderGraph::RenderGraph& graph);

        NO_COPY_CONSTRUCTORS(Renderer);
        NO_MOVE_CONSTRUCTORS(Renderer);

    private:
        void InitConfig();

    private:
        Config                   m_Config;
        Ref<Instance>            m_Instance;
        Ref<Debugger>            m_Debugger;
        Ref<Device>              m_Device;
        Ref<SemaphorePool>       m_SemaphorePool;
        ManualLifetime<Executor> m_Executor;
    };

}  // namespace Engine::Vulkan