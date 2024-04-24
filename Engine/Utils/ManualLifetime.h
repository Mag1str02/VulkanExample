#pragma once

#include "Assert.h"
#include "Constructors.h"

#include <type_traits>

namespace Engine {

    template <typename T>
    class ManualLifetime {
    public:
        template <typename... Args>
        ManualLifetime(Args&&... args) {
            Construct(std::forward<Args>(args)...);
        }
        ManualLifetime() = default;
        ~ManualLifetime() {
            if (m_Consturcted) {
                Destruct();
            }
        }

        NO_COPY_CONSTRUCTORS(ManualLifetime);
        NO_MOVE_CONSTRUCTORS(ManualLifetime);

        template <typename... Args>
        void Construct(Args&&... args) {
            m_Consturcted = true;
            new (&m_Storage) T(std::forward<Args>(args)...);
        }
        void Destruct() {
            DE_ASSERT(m_Consturcted, "Object is not created");
            reinterpret_cast<T*>(&m_Storage)->~T();
            m_Consturcted = false;
        }

        T* operator->() {
            DE_ASSERT(m_Consturcted, "Object is not created");
            return reinterpret_cast<T*>(&m_Storage);
        }
        const T* operator->() const {
            DE_ASSERT(m_Consturcted, "Object is not created");
            return reinterpret_cast<const T*>(&m_Storage);
        }

        T* Get() {
            DE_ASSERT(m_Consturcted, "Object is not created");
            return reinterpret_cast<T*>(&m_Storage);
        }
        const T* Get() const {
            DE_ASSERT(m_Consturcted, "Object is not created");
            return reinterpret_cast<const T*>(&m_Storage);
        }

        bool IsConstructed() const {
            return m_Consturcted;
        }

    private:
        alignas(T) std::byte m_Storage[sizeof(T)];
        bool m_Consturcted = false;
    };

}  // namespace Engine
