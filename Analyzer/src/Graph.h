#pragma once
#include <cmath>
#include <vector>
#include <limits>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <type_traits>

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Graph
{
private:
    std::vector<T> m_Xs;
    std::vector<T> m_Ys;
    size_t m_LastCleanupCount = 0;
private:
    inline double GetDeriviation()
    {
        double der = 0;
        for (size_t i = 0; i < m_Ys.size() - 1; ++i)
            der += std::abs(m_Ys[i] - m_Ys[i + 1]);
        return der / (double)m_Ys.size();
    }
public:
    inline Graph()
    {
        m_Xs.reserve(1000); // arbitrarily chosen
        m_Ys.reserve(1000);
    }

    inline void Cleanup(bool onlySame)
    {
        double deriviation = 0.0;
        if (!onlySame)
            deriviation = GetDeriviation();
        for (size_t i = 1; i < m_Ys.size() - 1; ++i)
        {
            if (std::abs(m_Ys[i] - m_Ys[i + 1]) <= deriviation && std::abs(m_Ys[i] - m_Ys[i - 1]) <= deriviation)
            {
                m_Ys.erase(m_Ys.begin() + (ptrdiff_t)i);
                m_Xs.erase(m_Xs.begin() + (ptrdiff_t)i);
                --i;
            }
        }
        m_LastCleanupCount = m_Ys.size();
    }

    inline void Add(T x, T y)
    {
        m_Xs.push_back(x);
        m_Ys.push_back(y);
    }

    inline const T* GetX() const { return m_Xs.data(); }
    inline const T* GetY() const { return m_Ys.data(); }
    inline int GetCount() const { return static_cast<int>(m_Xs.size()); }
    inline size_t GetGrowthSinceLastCleanup() const { return m_Ys.size() - m_LastCleanupCount; }
};