#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <string_view>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Plot.h"
#include "Serial.h"
#include "Window.h"
#include "Clang.h"
#include "Log.h"
#include "SettingsWindow.h"
#include "Profiler.h"


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
        plot.RenderLines();
        ImPlot::EndPlot();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}


const std::vector<std::string_view>& SplitStringByNl(const std::string_view& str)
{
    static std::vector<std::string_view> views;
    views.clear();

    size_t start = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '\n' && i - start > 0)
        {
            views.push_back({ &str[start], i - start });
            start = i+1;
        }
    }
    return views;
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
        data += readData;
        if (const size_t index = data.find_last_of('\n'); index != std::string::npos && !readData.empty())
        {
            const std::vector<std::string_view>& vec = SplitStringByNl(data);
            for (const auto& str : vec)
            {
                char* endptr;
                const size_t colonIdx = str.find(':');
                const std::string_view valueStr = str.substr(colonIdx + 1);
                const double value = std::strtod(valueStr.data(), &endptr);
                if (endptr == valueStr.data())
                    continue;

                static std::string graphName;
                graphName = str.substr(0, colonIdx);
                plot.Add(graphName, serial.GetTimeSinceStart(), value);
            }
            data.assign(&data[index+1]);
        }
        LinePlot(window.GetSize(), plot);
        window.EndFrame();
    }
    return 0;
}