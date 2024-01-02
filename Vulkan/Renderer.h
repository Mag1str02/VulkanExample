#pragma once

#include "Base.h"
#include "Instance.h"

namespace Vulkan {

    class Renderer {
    public:
        Renderer();
        ~Renderer();

        NO_COPY_CONSTRUCTORS(Renderer);
        NO_MOVE_CONSTRUCTORS(Renderer);

    private:
        Ref<Instance> m_Instance;
    };

}  // namespace Vulkan