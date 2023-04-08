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


std::vector<std::string_view> SplitStringByChar(const std::string_view& str, char c)
{
    std::vector<std::string_view> views;
    size_t start = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == c)
        {
            if(i - start > 0)
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

        //const std::string_view readData = serial.ReadData();
        static int i = 0;
        auto readData = std::to_string(i) + "\np:l:a:7\n";
        ++i;
        data += readData;
        if (const size_t index = data.find_last_of('\n'); index != std::string::npos && !readData.empty())
        {
            std::vector<std::string_view> vecData = SplitStringByChar(data, '\n');
            for (const auto& str : vecData)
            {
                static std::string plotName;
                static std::string graphName;
                std::string_view yLabel;
                std::vector<std::string_view> vec = SplitStringByChar(str, ':');

                char* endptr;
                std::string_view valueStr = str;
                if (const size_t valueIdx = str.find_last_of(':'); valueIdx != std::string::npos)
                    valueStr = str.substr(valueIdx + 1);
                const double value = std::strtod(valueStr.data(), &endptr);
                if (endptr == valueStr.data() || std::isinf(value) || std::isnan(value))
                    continue;
                
                switch (vec.size())
                {
                case 0:
                    plotName = "##default";
                    yLabel = "y";
                    graphName = "f(x)";
                    break;
                case 1:
                    plotName = "##default";
                    yLabel = "y";
                    graphName = vec[0];
                    break;
                case 2:
                    plotName = "##default";
                    yLabel = vec[0];
                    graphName = vec[1];
                    break;
                case 3:
                    plotName = vec[0];
                    yLabel = vec[1];
                    graphName = vec[2];
                    break;
                default:
                    continue;
                }
                plots.Add(plotName, yLabel, graphName, serial.GetTimeSinceStart(), value);
            }
            data.assign(&data[index+1]);
        }
        plots.CleanupGraphs();
        plots.Render(window.GetSize(), settings.DebugInfoSelected());
        window.EndFrame();
    }
    Thread::Join();
    return 0;
}