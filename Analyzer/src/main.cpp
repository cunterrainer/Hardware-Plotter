#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Plot.h"
#include "Serial.h"
#include "Window.h"
#include "Clang.h"
#include "Log.h"
#include "SettingsWindow.h"


template <class T>
void LinePlot(ImVec2 windowSize, const Plot<T>& plot)
{
    ImGui::SetNextWindowSize({ windowSize.x, windowSize.y - 43.f});
    ImGui::SetNextWindowPos({ 0, 43.f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##Plotter", nullptr, IMGUI_WINDOW_FLAGS);
    
    if (ImPlot::BeginPlot("##Line Plots", {-1,-1}))
    {
        ImPlot::SetupAxes("t in s", "y", ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_Y1, plot.GetYMin(), plot.GetYMax(), ImPlotCond_Always);
        //Log << plot.GetYMin() << " | " << plot.GetYMax() << Endl;
        plot.RenderLines();
        //ImPlot::PlotLine("", plot.GetTimes(), plot.GetValues(), plot.GetCount());
        ImPlot::EndPlot();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}


std::string RemoveRedundantChars(const std::string& str)
{
    static constexpr std::string_view validChars = ".-:()[]{}";
    std::string n;
    for (char c : str)
    {
        if (std::isalnum(static_cast<unsigned char>(c)) || validChars.find(c) != std::string::npos)
            n += c;
    }
    return n;
}


std::vector<std::string> SplitStringByChar(const std::string& str)
{
    std::stringstream test(str);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(test, segment, '\n'))
    {
        segment = RemoveRedundantChars(segment);
        if(segment.size() > 0)
            seglist.push_back(segment);
    }
    if(str.back() != '\n')
        seglist.pop_back();
    return seglist;
}


int main()
{
    Serial::Serial serial;
    std::string data;
    Plot<double> plot;
    SettingsWindow settings;
    const Window window;

    while (window.IsOpen())
    {
        window.StartFrame();
        if (settings.ConnectClicked(window.GetSize().x))
        {
            if (!serial.Connect(settings.GetSelectedPort(), settings.GetSelectedBaudRate()))
            {
                Err << serial.GetLastErrorMsg() << Endl;
                MsgBoxError(serial.GetLastErrorMsg().data());
            }
            data.clear();
        }

        const std::string_view readData = serial.ReadData();
        if (!readData.empty())
        {
            data += readData;
            if (data.find('\n') != std::string::npos)
            {
                std::vector<std::string> vec = SplitStringByChar(data);
                for (const auto& str : vec)
                {
                    std::string graphName = str.substr(0, str.find(':'));
                    std::string value = str.substr(str.find(':') + 1);
                    if (value == "ovf" || value == "nan" || value == "inf")
                        continue;

                    size_t graph = plot.AddGraph(graphName);
                    plot.Add(graph, serial.GetTimeSinceStart(), std::stod(value));
                }
                size_t index = data.find_last_of('\n')+1;
                data = std::string(std::next(data.begin(), (ptrdiff_t)index), data.end());
            }
        }
        LinePlot(window.GetSize(), plot);
        window.EndFrame();
    }
    return 0;
}