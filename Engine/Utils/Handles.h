#pragma once

/*
This is basicly another reinvented bicycle.
Handles work same as shared pointers, but supports internal lifetime dependencies
*/

#include "Aliases.h"
#include "Assert.h"
#include "Constructors.h"

#include <concepts>
#include <vector>

namespace Engine {

    template <typename T>
    class Handle;
    class HandledStorage;

    template <typename T>
    class Handle {
    public:
        Handle() = default;
        Handle& operator=(std::nullptr_t);
        ~Handle();

        template <typename U>
            requires std::derived_from<U, T>
        Handle(const Handle<U>& other);
        template <typename U>
            requires std::derived_from<U, T>
        Handle(Handle<U>&& other);
        template <typename U>
            requires std::derived_from<U, T>
        Handle& operator=(const Handle<U>& other);
        template <typename U>
            requires std::derived_from<U, T>
        Handle& operator=(Handle<U>&& other);

        T* operator->();
        T* Get();

        void Reset();

        uint32_t RefCount() const;
        uint32_t ExtRefCount() const;
        uint32_t IntRefCount() const;

        template <typename U>
        void AddLifetimeDependency(const Handle<U>& other);

    private:
        Handle(HandledStorage* storage, T* object);

    private:
        template <typename U>
        friend class Handle;
        friend class HandledStorage;

        template <typename U, typename... Args>
            requires std::derived_from<U, HandledStorage>
        friend Handle<U> CreateHandledStorage(Args&&... args);

        HandledStorage* m_HandledStorage = nullptr;
        T*              m_Object         = nullptr;
    };

    class HandledStorage {
    public:
        HandledStorage() = default;
        virtual ~HandledStorage();

        NO_COPY_CONSTRUCTORS(HandledStorage);
        NO_MOVE_CONSTRUCTORS(HandledStorage);

    protected:
        template <typename T>
        Handle<T> CreateHandle(T* object);

    private:
        void IncExtRef();
        void DecExtRef();
        void IncIntRef();
        void DecIntRef();
        void AttemptDestroy();

        void AddLifetimeDependecy(HandledStorage* storage);

    private:
        template <typename T>
        friend class Handle;

        uint32_t m_ExternalReferenceCount = 0;
        uint32_t m_InternalReferenceCount = 0;

        std::vector<HandledStorage*> m_LifetimeDependencies;
    };

    template <typename T, typename... Args>
        requires std::derived_from<T, HandledStorage>
    Handle<T> CreateHandledStorage(Args&&... args);

}  // namespace Engine

namespace Engine {

    template <typename T>
    Handle<T>::Handle(HandledStorage* storage, T* object) : m_HandledStorage(storage), m_Object(object) {
        m_HandledStorage->IncExtRef();
    }

    template <typename T>
    Handle<T>::~Handle() {
        Reset();
    }

    template <typename T>
    template <typename U>
        requires std::derived_from<U, T>
    Handle<T>::Handle(const Handle<U>& other) : m_HandledStorage(other.m_HandledStorage), m_Object(other.m_Object) {
        if (m_HandledStorage) {
            m_HandledStorage->IncExtRef();
        }
    }

    template <typename T>
    template <typename U>
        requires std::derived_from<U, T>
    Handle<T>::Handle(Handle<U>&& other) : m_HandledStorage(other.m_HandledStorage), m_Object(other.m_Object) {
        other.m_HandledStorage = nullptr;
        other.m_Object         = nullptr;
    }

    template <typename T>
    template <typename U>
        requires std::derived_from<U, T>
    Handle<T>& Handle<T>::operator=(const Handle<U>& other) {
        m_Object = other.m_Object;
        if (m_HandledStorage == other.m_HandledStorage) {
            return *this;
        }

        if (m_HandledStorage) {
            m_HandledStorage->DecExtRef();
        }
        m_HandledStorage = other.m_HandledStorage;
        if (m_HandledStorage) {
            m_HandledStorage->IncExtRef();
        }
    }

    template <typename T>
    template <typename U>
        requires std::derived_from<U, T>
    Handle<T>& Handle<T>::operator=(Handle<U>&& other) {
        m_Object = other.m_Object;
        if (&other == this) {
            return *this;
        }
        if (m_HandledStorage) {
            m_HandledStorage->DecExtRef();
        }
        m_HandledStorage = other.m_HandledStorage;

        other.m_HandledStorage = nullptr;
        other.m_Object         = nullptr;
        return *this;
    }

    template <typename T>
    Handle<T>& Handle<T>::operator=(std::nullptr_t) {
        Reset();
        return *this;
    }

    template <typename T>
    T* Handle<T>::operator->() {
        DE_ASSERT(m_Object != nullptr, "Dereferencing empty handle");
        return m_Object;
    }

    template <typename T>
    T* Handle<T>::Get() {
        return m_Object;
    }

    template <typename T>
    uint32_t Handle<T>::RefCount() const {
        if (!m_HandledStorage) {
            return 0;
        }
        return m_HandledStorage->m_ExternalReferenceCount + m_HandledStorage->m_InternalReferenceCount;
    }

    template <typename T>
    uint32_t Handle<T>::ExtRefCount() const {
        if (!m_HandledStorage) {
            return 0;
        }
        return m_HandledStorage->m_ExternalReferenceCount;
    }

    template <typename T>
    uint32_t Handle<T>::IntRefCount() const {
        if (!m_HandledStorage) {
            return 0;
        }
        return m_HandledStorage->m_InternalReferenceCount;
    }

    template <typename T>
    void Handle<T>::Reset() {
        if (m_HandledStorage) {
            m_HandledStorage->DecExtRef();
        }
        m_HandledStorage = nullptr;
        m_Object         = nullptr;
    }

    template <typename T>
    Handle<T> HandledStorage::CreateHandle(T* object) {
        return Handle<T>(this, object);
    }

    template <typename T>
    template <typename U>
    void Handle<T>::AddLifetimeDependency(const Handle<U>& other) {
        if (other.m_HandledStorage == nullptr || m_HandledStorage == nullptr) {
            return;
        }
        DE_ASSERT(other.m_HandledStorage != m_HandledStorage, "Cannot add container to itself as a lifetime dependency");
        m_HandledStorage->AddLifetimeDependecy(other.m_HandledStorage);
    }

    template <typename T, typename... Args>
        requires std::derived_from<T, HandledStorage>
    Handle<T> CreateHandledStorage(Args&&... args) {
        T* storage = new T(std::forward<Args>(args)...);
        return Handle<T>(storage, storage);
    }

}  // namespace Engine