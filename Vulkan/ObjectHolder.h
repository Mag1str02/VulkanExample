#pragma once

#include "Object.h"

namespace Vulkan {

    class ObjectHolder {
    public:
        ObjectHolder()          = default;
        virtual ~ObjectHolder() = default;

        void AddObject(Ref<Object> object);
        void Release();

    private:
        std::vector<Ref<Object>> m_HoldedObjects;
    };
}  // namespace Vulkan