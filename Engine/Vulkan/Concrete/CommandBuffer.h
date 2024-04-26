#pragma once

#include "Engine/Vulkan/Managed/CommandBuffer.h"

namespace Engine::Vulkan::Concrete {

    class CommandBuffer : public Managed::CommandBuffer {
    public:
        static Ref<CommandBuffer> Create(Ref<CommandPool> pool);
        ~CommandBuffer();

        void Begin();
        void End();
        void Reset();


        NO_COPY_CONSTRUCTORS(CommandBuffer);
        NO_MOVE_CONSTRUCTORS(CommandBuffer);

    private:
        CommandBuffer(Ref<CommandPool> pool);
    };

}  // namespace Engine::Vulkan::Concrete