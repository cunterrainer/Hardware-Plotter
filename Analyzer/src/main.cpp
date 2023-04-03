#include <thread>

#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

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


int main()
{
    const Window window;
    while (window.IsOpen())
    {
        window.StartFrame();
        window.EndFrame();
    }
    return 0;
}