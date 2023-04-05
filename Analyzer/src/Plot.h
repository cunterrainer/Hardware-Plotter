#pragma once
#include <vector>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <type_traits>
#include <unordered_map>

#include "ImPlot/implot.h"

#include "Graph.h"

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Plot
{
private:
    std::vector<Graph<T>> m_Graphs;
    std::unordered_map<std::string, size_t> m_GraphsNameMap; // maps a name to the index
    mutable double m_YMax = std::numeric_limits<double>::lowest();
    mutable double m_YMin = std::numeric_limits<double>::max();
public:
    inline size_t AddGraph(const std::string& name)
    {
        if (name != "log(n)" && name != "-log(n)")
            Log << "bad\n";
        const std::unordered_map<std::string, size_t>::const_iterator it = m_GraphsNameMap.find(name);
        if (it == m_GraphsNameMap.end())
        {
            static size_t idx = 0;
            m_Graphs.push_back(Graph<T>(name));
            m_GraphsNameMap[name] = idx;
            return idx++;
        }
        return it->second;
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
            ImPlot::PlotLine(graph.GetName(), graph.GetX(), graph.GetY(), graph.GetCount());
    }

    inline double GetYMax() const { return m_YMax; }
    inline double GetYMin() const { return m_YMin; }
};