#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <string_view>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Serial.h"
#include "Plot.h"
#include "Window.h"
#include "Clang.h"
#include "Log.h"
#include "SettingsWindow.h"
#include "Profiler.h"
#include "PlotManager.h"


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
    SettingsWindow settings;
    PlotManager plots(settings.GetHeight());
    const Window window;

    while (window.IsOpen())
    {
        window.StartFrame();
        if (settings.ConnectClicked(window.GetSize().x, serial.IsConnected()))
        {
            if (serial.IsConnected())
                serial.Disconnect();
            // try to connect
            else if (!serial.Connect(settings.GetSelectedPort(), settings.GetSelectedBaudRate()))
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
                const size_t valueIdx = str.find_last_of(':');
                const std::string_view valueStr = str.substr(valueIdx + 1);
                const double value = std::strtod(valueStr.data(), &endptr);
                if (endptr == valueStr.data())
                    continue;

                const size_t yLabelIdx = str.find_first_of(':');
                const size_t graphIdx = str.find_first_of(':', yLabelIdx+1);
                static std::string plotName;
                static std::string graphName;
                std::string_view ylabel = str.substr(yLabelIdx + 1, graphIdx - yLabelIdx - 1);
                plotName = str.substr(0, yLabelIdx);
                graphName = str.substr(graphIdx + 1, valueIdx-graphIdx-1);
                plots.Add(plotName, ylabel, graphName, serial.GetTimeSinceStart(), value);
            }
            data.assign(&data[index+1]);
        }
        if(settings.CleanupGraphs())
            plots.CleanupGraphs(settings.CleanupGraphsSame());
        plots.Render(window.GetSize());
        window.EndFrame();
    }
    return 0;
}