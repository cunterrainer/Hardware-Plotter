#include <string>
#include <optional>
#include <utility>
#include <algorithm>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Serial.h"
#include "Window.h"
#include "Clang.h"
#include "Log.h"


template <class T>
void LinePlot(const Window& w, T val, T ti)
{
    ImGui::SetNextWindowSize({ w.GetSize().x, w.GetSize().y - 43.f});
    ImGui::SetNextWindowPos({ 0, 43.f });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("##Plotter", nullptr, IMGUI_WINDOW_FLAGS);

    static std::vector<T> value;
    static std::vector<T> time;
    static T greatestValue = (std::numeric_limits<T>::min)();
    static T lowestValue = (std::numeric_limits<T>::max)();
    static double yOffset = 0;
    static double yMax = 0;
    static double yMin = 0;
    if (val != -1)
    {
        greatestValue = (std::max)(greatestValue, val);
        lowestValue = (std::min)(lowestValue, val);
        yOffset = (std::max)(std::abs(greatestValue * 0.05), std::abs(lowestValue * 0.05));
        if (greatestValue >= 0)
            yMax = greatestValue + yOffset;
        else
            yMax = greatestValue - yOffset;

        if (lowestValue >= 0)
            yMin = lowestValue - yOffset;
        else
            yMin = lowestValue + yOffset;
        if (time.empty())
        {
            value.push_back(val);
            time.push_back(ti);
        }
        else if(time.back() != ti)
        {
            value.push_back(val);
            time.push_back(ti);
        }
    }
    
    if (ImPlot::BeginPlot("Line Plots", {-1,-1}))
    {
        ImPlot::SetupAxes("t in s", "y", ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImPlotCond_Always);
        ImPlot::PlotLine("value1", time.data(), value.data(), value.size());
        ImPlot::EndPlot();
    }
    ImGui::End();
    ImGui::PopStyleVar();
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
    return std::move(serial);
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
            if (serial.IsConnected())
                serial.Disconnect();

            std::optional<Serial::Serial> optSerial = Connect(ports[(size_t)selectedPort].com, selectedRate);
            if (optSerial.has_value())
                serial = std::move(optSerial.value());
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("##PortCombo", &selectedPort, comboboxContent.c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("baud", &selectedRate, Serial::Serial::BaudRates.data());
        ImGui::End();

        int c = -1;
        if (serial.IsConnected())
        {
            std::string e = serial.ReadData();
            if (!e.empty())
            {
                try
                {
                    c = stoi(e);
                    //Log << stoi(e) << " | " << serial.GetTimeSinceStart() << Endl;
                    //LinePlot(window, std::stoi(e), (int)serial.GetTimeSinceStart());
                }
                catch (...)
                {
                    c = -1;
                }
            }
            LinePlot(window, (double)c, serial.GetTimeSinceStart());
        }
        window.EndFrame();
    }
    return 0;
}