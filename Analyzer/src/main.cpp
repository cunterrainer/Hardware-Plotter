#include "ImGui/imgui.h"
#include "ImPlot/implot.h"

#include "Window.h"
#include "Clang.h"
#include "Log.h"


int main()
{
    const Window window;
    while (window.IsOpen())
    {
        window.StartFrame();
        ImPlot::ShowDemoWindow();
        ImGui::ShowMetricsWindow();
        window.EndFrame();
    }
    return 0;
}