#include "Handles.h"

namespace Engine {

    HandledStorage::~HandledStorage() {
        for (const auto& storage : m_LifetimeDependencies) {
            storage->DecIntRef();
        }
    }

    void HandledStorage::IncExtRef() {
        ++m_ExternalReferenceCount;
    }
    void HandledStorage::DecExtRef() {
        --m_ExternalReferenceCount;
        AttemptDestroy();
    }
    void HandledStorage::IncIntRef() {
        ++m_InternalReferenceCount;
    }
    void HandledStorage::DecIntRef() {
        --m_InternalReferenceCount;
        AttemptDestroy();
    }
    void HandledStorage::AttemptDestroy() {
        if (m_InternalReferenceCount == 0 && m_ExternalReferenceCount == 0) {
            // Spooooky
            delete this;
        }
    }

    void HandledStorage::AddLifetimeDependecy(HandledStorage* storage) {
        m_LifetimeDependencies.push_back(storage);
        storage->IncIntRef();
    }
}  // namespace Engine