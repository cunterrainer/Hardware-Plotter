#include <sstream>
#include <iostream>
#include <functional>

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for separator, gauge, text, Element, operator|, vbox, border
#include "ftxui/component/loop.hpp"

#include "Serial.h"
#include "Thread.h"
#include "PlotManager.h"

using namespace ftxui;

ButtonOption ButtonStyle() {
    auto option = ButtonOption::Animated();
    option.transform = [](const EntryState& s) 
        {
            auto element = text(s.label);
            if (s.focused)
                element |= bold;
            return element | center | borderEmpty | flex;
        };
    return option;
}

std::vector<std::string> SplitString(std::string_view str, char c)
{
    std::stringstream ss;
    ss << std::string(str.data(), 144);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(ss, segment, c))
        seglist.push_back(segment);
    return seglist;
}

void ConnectSerial(std::string& data, Serial::Serial& serial, PlotManager& plots, PortSetup& portSetup)
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
}


std::vector<std::string_view> SplitStringByChar(const std::string_view& str, char c)
{
    std::vector<std::string_view> views;
    size_t start = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == c)
        {
            if (i - start > 0)
                views.push_back({ &str[start], i - start });
            start = i + 1;
        }
    }
    return views;
}


void ReadSerialData(std::string& data, Serial::Serial& serial, PlotManager& plots)
{
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
        data.assign(&data[index + 1]);
    }
}


int main()
{
    std::string data;
    Serial::Serial serial;
    PlotManager plots(RenderWindow::SettingsHeight);
    PortSetup portSetup;
    const std::vector<std::string> baudRates = SplitString(Serial::Serial::BaudRates, '\0');
    auto ConnectSerialBound = std::bind(ConnectSerial, std::ref(data), std::ref(serial), std::ref(plots), std::ref(portSetup));
    auto readData = std::bind(ReadSerialData, std::ref(data), std::ref(serial), std::ref(plots));

    auto buttons = Container::Horizontal({ Button("Connect", ConnectSerialBound), Dropdown(&portSetup.PortsVector(), &portSetup.SelectedPort()), Dropdown(&baudRates, &portSetup.SelectedBaudRate()), Button("Update ports", [&] { portSetup.UpdatePorts(); portSetup.PortsVector(); }) });
 
    // Modify the way to render them on screen:
    auto component = Renderer(buttons, [&] { return vbox({ buttons->Render() }) | border; });

    auto screen = ScreenInteractive::Fullscreen();
    Loop loop(&screen, std::move(component));
    while (!loop.HasQuitted())
    {
        ReadSerialData(data, serial, plots);
        loop.RunOnceBlocking();
    }

    Thread::Join();
    std::cin.get();
}