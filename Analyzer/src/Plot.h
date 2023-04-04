#pragma once
#include <vector>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <type_traits>

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Plot
{
private:
    static constexpr double YPercentageScalar = 0.05;
private:
    std::vector<T> m_Values;
    std::vector<T> m_Times;
    T m_GreatestValue = std::numeric_limits<T>::min();
    T m_LowestValue = std::numeric_limits<T>::max();
    T m_YMax = 0;
    T m_YMin = 0;
private:
    inline void CalculateYRange()
    {
        const double yOffset = std::max(std::abs(m_GreatestValue * YPercentageScalar), std::abs(m_LowestValue * YPercentageScalar));
        if (m_GreatestValue >= 0)
            m_YMax = m_GreatestValue + yOffset;
        else
            m_YMax = m_GreatestValue - yOffset;

        if (m_LowestValue >= 0)
            m_YMin = m_LowestValue - yOffset;
        else
            m_YMin = m_LowestValue + yOffset;
    }
public:
    inline Plot()
    {
        m_Values.reserve(1000); // arbitrarily chosen
        m_Times.reserve(1000);
    }

    inline void Add(T value, T time)
    {
        m_GreatestValue = std::max(m_GreatestValue, value);
        m_LowestValue = std::min(m_LowestValue, value);
        CalculateYRange();
        m_Values.push_back(value);
        m_Times.push_back(time);
    }

    inline const T* GetTimes()  const { return m_Times.data(); }
    inline const T* GetValues() const { return m_Values.data(); }
    inline T GetYMax() const { return m_YMax; }
    inline T GetYMin() const { return m_YMin; }
    inline int GetCount() const { return static_cast<int>(m_Times.size()); }
};