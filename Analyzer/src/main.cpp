#include <thread>

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


int W()
{
    const Window window;
    while (window.IsOpen())
    {
        window.StartFrame();
        window.EndFrame();
    }
    return 0;
}


int main()
{
    Serial serial("\\\\.\\COM3");
    //Serial* SP = new Serial("\\\\.\\COM3");    // adjust as needed

    if (serial.IsConnected())
        Log << "Serial connected" << Endl;

    char incomingData[256] = "";
    int dataLength = 255;
    int readResult = 0;

    while (serial.IsConnected())
    {
        readResult = serial.ReadData(incomingData, dataLength);
        incomingData[readResult] = 0;

        printf("%s", incomingData);

        //Sleep(500);
    }
    return 0;
}