#pragma once

#include "Engine/Vulkan/Managed/RawCommandBuffer.h"

namespace Engine::Vulkan {

    class RawCommandBuffer : public Managed::RawCommandBuffer, NonCopyMoveable {
    public:
        static Ref<RawCommandBuffer> Create(Ref<CommandPool> pool, bool secondary = false);
        ~RawCommandBuffer();

    private:
        RawCommandBuffer(Ref<CommandPool> pool, bool secondary);

    private:
        Ref<CommandPool> m_Pool;
    };

}  // namespace Engine::Vulkan