#pragma once
#include <cmath>
#include <vector>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <type_traits>

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Graph
{
private:
    static constexpr double YPercentageScalar = 0.05;
private:
    std::vector<T> m_Xs;
    std::vector<T> m_Ys;
    T m_GreatestY = std::numeric_limits<T>::lowest();
    T m_LowestY   = std::numeric_limits<T>::max();
    double m_YMax = std::numeric_limits<double>::lowest();
    double m_YMin = std::numeric_limits<double>::max();
    size_t m_PreviousEnd = 1;
    size_t m_LastCleanupCount = 0;
private:
    inline void CalculateYRange()
    {
        const double yOffset = -std::max(std::abs(m_GreatestY * YPercentageScalar), std::abs(m_LowestY * YPercentageScalar));
        m_YMax = m_GreatestY - yOffset;
        m_YMin = m_LowestY + yOffset;
    }
public:
    inline Graph()
    {
        m_Xs.reserve(1000); // arbitrarily chosen
        m_Ys.reserve(1000);
    }

    inline void Cleanup()
    {
        // [1,2,4,5]
        // [1,2,4,5,6]
        //std::cout << "prev size: " << m_Ys.size();
        for (size_t i = m_PreviousEnd; i < m_Ys.size()-2; ++i)
        {
            if (std::abs(m_Ys[i] - m_Ys[i + 1]) < 0.1)
            {
                m_Ys.erase(m_Ys.begin() + i + 1);
                m_Xs.erase(m_Xs.begin() + i + 1);
            }
        }
        m_PreviousEnd = m_Ys.size() - 1;
        m_LastCleanupCount = m_Ys.size();
        //std::cout << " current size: " << m_Ys.size() << std::endl;
    }

    inline void Add(T x, T y)
    {
        m_GreatestY = std::max(m_GreatestY, y);
        m_LowestY = std::min(m_LowestY, y);
        CalculateYRange();
        m_Xs.push_back(x);
        m_Ys.push_back(y);
    }

    inline const T* GetX() const { return m_Xs.data(); }
    inline const T* GetY() const { return m_Ys.data(); }
    inline double GetYMax() const { return m_YMax; }
    inline double GetYMin() const { return m_YMin; }
    inline int GetCount() const { return static_cast<int>(m_Xs.size()); }
    inline size_t GetGrowthSinceLastCleanup() const { return m_Ys.size() - m_LastCleanupCount; }
};