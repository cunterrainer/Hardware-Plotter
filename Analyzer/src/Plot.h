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

#include "RenderWindow.h"
#include "Graph.h"
#include "ImageWriter.h"

template <class T, std::enable_if_t<std::numeric_limits<T>::is_integer || std::is_floating_point_v<T>, bool> = true>
class Plot
{
private:
    static constexpr double YPercentageScalar = 0.05;
private:
    // using a map because I profiled it with std::vector and the map was faster
    std::unordered_map<std::string, Graph<T>> m_Graphs;
    std::string m_YLabel;
    bool m_LabelAssigned = false;
    bool m_CleanupGraphs = false;
    bool m_CleanupOnlySame = false;
    bool m_SaveClicked = false;
    bool m_Pause = false;
    T m_YMax = std::numeric_limits<T>::lowest();
    T m_YMin = std::numeric_limits<T>::max();
    T m_GreatestY = std::numeric_limits<T>::lowest();
    T m_LowestY = std::numeric_limits<T>::max();
    Image m_Image;
    ImageWriter m_ImageWriter{ &m_Image };
private:
    inline void CalculateYRange()
    {
        const double yOffset = -std::max(std::abs(m_GreatestY * YPercentageScalar), std::abs(m_LowestY * YPercentageScalar));
        m_YMax = m_GreatestY - yOffset;
        m_YMin = m_LowestY + yOffset;
    }
public:
    inline void Add(const std::string& graphName, std::string_view ylabel, T x, T y)
    {
        if (m_Pause)
            return;
        if (!m_LabelAssigned)
        {
            m_YLabel = ylabel;
            m_LabelAssigned = true;
        }
        Graph<T>& graph = m_Graphs[graphName];
        graph.Add(x, y);
        m_GreatestY = std::max(m_GreatestY, y);
        m_LowestY = std::min(m_LowestY, y);
        CalculateYRange();
    }


    inline const Image& Capture(ImVec2 size, float yOffset)
    {
        m_Image.CaptureScreen({ size.x, size.y - 30 }, { 0, ImGui::GetIO().DisplaySize.y - size.y - yOffset + 2});
        return m_Image;
    }


    inline void ResetImage()
    {
        m_Image.Reset();
    }


    inline void Render(ImVec2 size, float yOffset, const char* plotName, bool debugInfo)
    {
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowPos({ 0, yOffset });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin(plotName, nullptr, IMGUI_WINDOW_FLAGS);
        ImGui::SameLine(8);
        RenderWindow::SetButtonRed(m_Pause);
        if (ImGui::Button(m_Pause ? "Continue" : "Pause", {150, 0}))
            m_Pause = !m_Pause;
        RenderWindow::ResetButtonColor();
        ImGui::SameLine();
        if (ImGui::Button("Save", { 150,0 }) || m_ImageWriter.IsOpen())
        {
            m_Image.Create({ size.x, size.y - 30 }, { 0, ImGui::GetIO().DisplaySize.y - size.y - yOffset + 2});
            if (m_ImageWriter.SaveImage())
            {
                ResetImage();
                m_ImageWriter.Close();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear", { 150, 0 }))
            m_Graphs.clear();
        ImGui::SameLine();
        ImGui::Checkbox("Cleanup Graphs", &m_CleanupGraphs);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Cleanup graphs by removing elements that have a similar y value.\n(Improves performance, however you may encounter bugs or the graphs might not look like what you expect)");
        ImGui::SameLine();
        if (ImGui::Checkbox("Cleanup only same", &m_CleanupOnlySame) && m_CleanupOnlySame)
            m_CleanupGraphs = true;
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Cleanup the graph but only if the y values are the same");

        if (ImPlot::BeginPlot(plotName, {-1, -1}))
        {
            ImPlot::SetupAxes("t in s", m_YLabel.c_str(), ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);
            ImPlot::SetupAxisLimits(ImAxis_Y1, m_YMin, m_YMax, ImPlotCond_Always);
            if (debugInfo)
                for (auto it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
                    ImPlot::PlotLine(std::string(std::string(it->first.c_str()) + ":" + std::to_string(it->second.GetCount())).c_str(), it->second.GetX(), it->second.GetY(), it->second.GetCount());
            else
                for (auto it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
                    ImPlot::PlotLine(it->first.c_str(), it->second.GetX(), it->second.GetY(), it->second.GetCount());
            ImPlot::EndPlot();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }


    inline void CleanupGraphs()
    {
        if (!m_CleanupGraphs) return;
        for (auto it = m_Graphs.begin(); it != m_Graphs.end(); ++it)
        {
            if (it->second.GetGrowthSinceLastCleanup() > 100) // there's no specific reason for it to be 100
                it->second.Cleanup(m_CleanupOnlySame);
        }
    }
};