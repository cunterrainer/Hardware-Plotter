#pragma once
#include <string>
#include <unordered_map>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Plot.h"

class PlotManager
{
private:
    std::unordered_map<std::string, Plot<double>> m_Plots;
    float m_YOffset;
public:
    inline explicit PlotManager(float yOffset) : m_YOffset(yOffset) {}

    inline void Add(const std::string& plotName, const std::string& graphName, double x, double y)
    {
        m_Plots[plotName].Add(graphName, x, y);
    }

    inline void Render(ImVec2 windowSize)
    {
        size_t count = 0;
        float height = (windowSize.y - m_YOffset) / (float)m_Plots.size();
        for (auto it = m_Plots.begin(); it != m_Plots.end(); ++it)
        {
            it->second.Render({ windowSize.x, height }, m_YOffset + height*(float)count, it->first.c_str());
            ++count;
        }
    }

    inline void CleanupGraphs(bool onlySame)
    {
        for (auto it = m_Plots.begin(); it != m_Plots.end(); ++it)
        {
            it->second.CleanupGraphs(onlySame);
        }
    }
};