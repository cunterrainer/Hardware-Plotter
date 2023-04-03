#include <string>

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


void Connect(const std::string& port, int selectedBaudRate)
{
    Serial::Serial serial(port, selectedBaudRate);
    if (!serial.IsConnected())
    {
        Err << serial.GetLastErrorMsg() << Endl;
        MsgBoxError(serial.GetLastErrorMsg().data());
        return;
    }
    
    char incomingData[256] = "";
    unsigned int dataLength = 255;
    int readResult = 0;

    while (serial.IsConnected())
    {
        readResult = serial.ReadData(incomingData, dataLength);
        incomingData[readResult] = 0;

        printf("%s", incomingData);
    }
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
            Connect(ports[(size_t)selectedPort].com, selectedRate);
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("##PortCombo", &selectedPort, comboboxContent.c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("baud", &selectedRate, Serial::Serial::BaudRates.data());
        ImGui::End();
        window.EndFrame();
    }
    return 0;
}