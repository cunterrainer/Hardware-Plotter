#include <string>
#include <optional>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>

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

    if (val != -1.0)
    {
        //Log << "Double: " << std::setprecision(17) << val << std::fixed << Endl;
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
        value.push_back(val);
        time.push_back(ti);
    }
    
    if (ImPlot::BeginPlot("Line Plots", {-1,-1}))
    {
        ImPlot::SetupAxes("t in s", "y", ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImPlotCond_Always);
        ImPlot::PlotLine("value1", time.data(), value.data(), (int)value.size());
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
                    //Log << "Original: " << data << " end" << Endl;
                    std::vector<std::string> vec = SplitStringByChar(data);
                    for (const auto& str : vec)
                    {
                        //Log << "Str: " << str << Endl;
                        LinePlot(window, std::stod(str), serial.GetTimeSinceStart());
                    }
                    ptrdiff_t index = (ptrdiff_t)data.find_last_of('\n')+1;
                    data = std::string(std::next(data.begin(), index), data.end());
                    //Log << "Data: " << data << " end" << Endl;
                }
            }
            LinePlot(window, -1.0, serial.GetTimeSinceStart());
        }
        window.EndFrame();
    }
    return 0;
}