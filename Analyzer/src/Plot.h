#pragma once
#include <limits>
#include <cmath>
#include <string>
#include <cstdlib>
#include <type_traits>
#include <unordered_map>

#include "ImPlot/implot.h"

#include "Graph.h"

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Plot
{
private:
    // using a map because I profiled it with std::vector and the map was faster
    std::unordered_map<std::string, Graph<T>> m_Graphs;
    mutable double m_YMax = std::numeric_limits<double>::lowest();
    mutable double m_YMin = std::numeric_limits<double>::max();
public:
    inline void Add(const std::string& graphName, T x, T y)
    {
        Graph<T>& graph = m_Graphs[graphName];
        graph.Add(x, y);
        m_YMax = std::max(m_YMax, graph.GetYMax());
        m_YMin = std::min(m_YMin, graph.GetYMin());
    }

    inline void RenderLines() const
    {
        for (auto it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
            ImPlot::PlotLine(it->first.c_str(), it->second.GetX(), it->second.GetY(), it->second.GetCount());
    }

    inline double GetYMax() const { return m_YMax; }
    inline double GetYMin() const { return m_YMin; }

    inline void CleanupGraphs(bool onlySame)
    {
        for (auto it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
        {
            if (it->second.GetGrowthSinceLastCleanup() > 100) // there's no specific reason for it to be 100
                it->second.Cleanup(onlySame);
        }
    }
};