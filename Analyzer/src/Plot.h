#pragma once
#include <limits>
#include <cmath>
#include <string>
#include <cstdlib>
#include <type_traits>
#include <string_view>
#include <unordered_map>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#undef max // windows macros
#undef min // windows macros
#include "Window.h"
#include "Graph.h"

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Plot
{
private:
    // using a map because I profiled it with std::vector and the map was faster
    std::unordered_map<std::string, Graph<T>> m_Graphs;
    std::string m_YLabel;
    bool m_LabelAssigned = false;
    mutable double m_YMax = std::numeric_limits<double>::lowest();
    mutable double m_YMin = std::numeric_limits<double>::max();
public:
    inline void Add(const std::string& graphName, std::string_view ylabel, T x, T y)
    {
        if (!m_LabelAssigned)
        {
            m_YLabel = ylabel;
            m_LabelAssigned = true;
        }
        Graph<T>& graph = m_Graphs[graphName];
        graph.Add(x, y);
        m_YMax = std::max(m_YMax, graph.GetYMax());
        m_YMin = std::min(m_YMin, graph.GetYMin());
    }


    inline void Render(ImVec2 size, float yOffset, const char* plotName) const
    {
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowPos({ 0, yOffset });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin(plotName, nullptr, IMGUI_WINDOW_FLAGS);

        if (ImPlot::BeginPlot(plotName, { -1,-1 }))
        {
            ImPlot::SetupAxes("t in s", m_YLabel.c_str(), ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);
            ImPlot::SetupAxisLimits(ImAxis_Y1, m_YMin, m_YMax, ImPlotCond_Always);
            for (auto it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
                ImPlot::PlotLine(it->first.c_str(), it->second.GetX(), it->second.GetY(), it->second.GetCount());
            ImPlot::EndPlot();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }


    inline void CleanupGraphs(bool onlySame)
    {
        for (auto it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
        {
            if (it->second.GetGrowthSinceLastCleanup() > 100) // there's no specific reason for it to be 100
                it->second.Cleanup(onlySame);
        }
    }
};