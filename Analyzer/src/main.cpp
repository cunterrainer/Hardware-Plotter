#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <string_view>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Serial.h"
#include "Plot.h"
#include "Clang.h"
#include "Log.h"
#include "RenderWindow.h"
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
    std::string data;
    Serial::Serial serial;
    PlotManager plots(RenderWindow::SettingsHeight);
    PortSetup portSetup;
    RenderWindow window;

    while (window.IsOpen())
    {
        window.Show(serial.IsConnected(), &portSetup);
        if (window.ConnectClicked())
        {
            if (serial.IsConnected())
            {
                serial.Disconnect();
                plots.Delete();
            }
            // try to connect
            else if (!serial.Connect(portSetup.Settings()))
            {
                Err << serial.GetLastErrorMsg() << Endl;
                MsgBoxError(serial.GetLastErrorMsg().data());
            }
            data.clear();
            window.ResetConnectClicked();
        }

        const std::string_view readData = serial.ReadData();
        data += readData;
        if (const size_t index = data.find_last_of('\n'); index != std::string::npos && !readData.empty())
        {
            const double elapsedTime = serial.GetTimeSinceStart();
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
                plots.Add(plotName, yLabel, graphName, elapsedTime, value);
            }
            data.assign(&data[index+1]);
        }
        plots.CleanupGraphs();
        plots.Render(window.Size(), window.ShowDebugInfo());
        if(window.SaveAllClicked())
            window.SaveAllClicked() = !plots.SaveAllPlots(window.Size());
        window.EndFrame();
    }
    Thread::Join();
    return 0;
}