#include <string>
#include <optional>
#include <utility>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Serial.h"
#include "Window.h"
#include "Clang.h"
#include "Log.h"


void LinePlot()
{
    //ImGui::SetNextWindowSize(window.GetSize());
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Test", nullptr, IMGUI_WINDOW_FLAGS);

    // f(x)=x^2
    static float x[101], y[101];
    for (int i = -50; i <= 50; ++i)
    {
        x[i+50] = (float)i;
        y[i+50] = (float)(i * i);
    }

    if (ImPlot::BeginPlot("Line Plots", {-1,-1}))
    {
        //ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 30);
        ImPlot::SetupAxes("x", "y");
        ImPlot::PlotLine("f(x)", x, y, 100);
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

        if (serial.IsConnected())
        {
            std::string e = serial.ReadData();
            if(!e.empty())
                printf("%s ", e.c_str());
        }
        window.EndFrame();
    }
    return 0;
}