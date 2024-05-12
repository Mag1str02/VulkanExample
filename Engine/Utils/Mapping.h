#pragma once

#include <concepts>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace Engine {

    template <typename A, typename B>
        requires(std::is_same_v<A, B> == false)
    class Mapping {
    public:
        void Clear() {
            m_FromA.clear();
            m_FromB.clear();
        }

        void AddMapping(A a, B b) {
            m_FromA[a].emplace(std::move(b));
            m_FromB[b].emplace(std::move(a));
        }

        bool Empty() const {
            return m_FromA.empty() && m_FromB.empty();
        }

        const std::unordered_set<A>& Get(B b) {
            static std::unordered_set<A> empty;
            if (auto it = m_FromB.find(b); it != m_FromB.end()) {
                return it->second;
            }
            return empty;
        }
        const std::unordered_set<B>& Get(A a) {
            static std::unordered_set<B> empty;
            if (auto it = m_FromA.find(a); it != m_FromA.end()) {
                return it->second;
            }
            return empty;
        }

        void RemoveMapping(A a, B b) {
            auto it_a = m_FromA.find(a);
            if (it_a != m_FromA.end()) {
                it_a->second.erase(b);
                if (it_a->second.empty()) {
                    m_FromA.erase(it_a);
                }
            }
            auto it_b = m_FromB.find(b);
            if (it_b != m_FromB.end()) {
                it_b->second.erase(a);
                if (it_b->second.empty()) {
                    m_FromB.erase(it_b);
                }
            }
        }

    private:
        std::unordered_map<A, std::unordered_set<B>> m_FromA;
        std::unordered_map<B, std::unordered_set<A>> m_FromB;
    };

}  // namespace Engine