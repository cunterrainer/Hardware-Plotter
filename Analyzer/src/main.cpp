#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Plot.h"
#include "Serial.h"
#include "Window.h"
#include "Clang.h"
#include "Log.h"


template <class T>
void LinePlot(ImVec2 windowSize, const Plot<T>& plot)
{
    ImGui::SetNextWindowSize({ windowSize.x, windowSize.y - 43.f});
    ImGui::SetNextWindowPos({ 0, 43.f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##Plotter", nullptr, IMGUI_WINDOW_FLAGS);
    
    if (ImPlot::BeginPlot("Line Plots", {-1,-1}))
    {
        ImPlot::SetupAxes("t in s", "y", ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_Y1, plot.GetYMin(), plot.GetYMax(), ImPlotCond_Always);
        ImPlot::PlotLine("value1", plot.GetTimes(), plot.GetValues(), plot.GetCount());
        ImPlot::EndPlot();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}


std::string RemoveRedundantChars(const std::string& str)
{
    std::string n;
    for (char c : str)
    {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '.')
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
        if (segment == "ovf" || segment == "nan" || segment == "inf")
            continue;
        if(segment.size() > 0)
            seglist.push_back(segment);
    }
    if(str.back() != '\n')
        seglist.pop_back();
    return seglist;
}


std::optional<Serial::Serial> Connect(const std::string& port, int selectedBaudRate)
{
    Serial::Serial serial(port, selectedBaudRate);
    if (!serial.IsConnected())
    {
        Err << serial.GetLastErrorMsg() << Endl;
        MsgBoxError(serial.GetLastErrorMsg().data());
        return std::nullopt;
    }
    return serial;
}


int main()
{
    const std::vector<Serial::Port> ports = Serial::PortListener::GetPorts();
    const std::string comboboxContent = [&]()
    {
        std::string content;
        for (const Serial::Port& port : ports)
            content += port.com + '\n' + port.device + '\0';
        return content;
    }();

    Serial::Serial serial;
    const Window window;
    static std::string data;
    Plot<double> plot;
    while (window.IsOpen())
    {
        window.StartFrame();
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({ window.GetSize().x, 43.f });
        ImGui::Begin("##Port selection", nullptr, IMGUI_WINDOW_FLAGS);
        static int selectedPort = 0;
        static int selectedRate = 0;
        if (ImGui::Button("Listen", {150, 0}))
        {
            serial.Disconnect();

            std::optional<Serial::Serial> optSerial = Connect(ports[(size_t)selectedPort].com, selectedRate);
            if (optSerial.has_value())
                serial = std::move(optSerial.value());
            data.clear();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("##PortCombo", &selectedPort, comboboxContent.c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("baud", &selectedRate, Serial::Serial::BaudRates.data());
        ImGui::SameLine(window.GetSize().x - 250);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        if (serial.IsConnected())
        {
            std::string e = serial.ReadData();
            if (!e.empty())
            {
                data += e;
                if (data.find('\n') != std::string::npos)
                {
                    Log << "Original: " << data << " end" << Endl;
                    std::vector<std::string> vec = SplitStringByChar(data);
                    for (const auto& str : vec)
                    {
                        Log << "Str: " << str << Endl;
                        plot.Add(std::stod(str), serial.GetTimeSinceStart());
                    }
                    ptrdiff_t index = (ptrdiff_t)data.find_last_of('\n')+1;
                    data = std::string(std::next(data.begin(), index), data.end());
                    Log << "Data: " << data << " end" << Endl;
                }
            }
            LinePlot(window.GetSize(), plot);
        }
        window.EndFrame();
    }
    return 0;
}