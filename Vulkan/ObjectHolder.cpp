#include "ObjectHolder.h"

namespace Vulkan {
    void ObjectHolder::AddObject(Ref<Object> object) {
        m_HoldedObjects.push_back(object);
    }
    void ObjectHolder::Release() {
        m_HoldedObjects.clear();
    }

}  // namespace Vulkan