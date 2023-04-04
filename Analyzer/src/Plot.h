#pragma once
#include <vector>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <type_traits>

#include "ImPlot/implot.h"

#include "Graph.h"

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Plot
{
private:
    std::vector<Graph<T>> m_Graphs;
    mutable double m_YMax = std::numeric_limits<double>::lowest();
    mutable double m_YMin = std::numeric_limits<double>::max();
public:
    inline void AddGraph()
    {
        m_Graphs.push_back({});
    }

    inline void Add(size_t graph, T x, T y)
    {
        m_Graphs[graph].Add(x, y);
        m_YMax = std::max(m_YMax, m_Graphs[graph].GetYMax());
        m_YMin = std::min(m_YMin, m_Graphs[graph].GetYMin());
    }

    inline void RenderLines() const
    {
        for(const auto& graph : m_Graphs)
            ImPlot::PlotLine("", graph.GetX(), graph.GetY(), graph.GetCount());
    }

    inline double GetYMax() const { return m_YMax; }
    inline double GetYMin() const { return m_YMin; }
};