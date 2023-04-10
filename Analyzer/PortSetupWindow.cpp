#include "PortSetupWindow.h"
#include "RenderWindow.h"

void PortSetupWindow::Show(std::string_view ports)
{
    RenderWindow::SetThemePopup();
    Open = true;
    const ImGuiIO& io = ImGui::GetIO();
    const ImVec2 pos{ (io.DisplaySize.x - WindowSize.x) / 2.f, (io.DisplaySize.y - WindowSize.y) / 2.f };
    const ImVec2 mousePos = ImGui::GetMousePos();
    if (mousePos.x < pos.x || mousePos.x > pos.x + WindowSize.x || mousePos.y < pos.y || mousePos.y > pos.y + WindowSize.y)
        ImGui::SetNextWindowFocus();
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(WindowSize);
    ImGui::Begin("Port Setup", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    PlaceText(DeviceStr);
    ImGui::Combo("##DeviceCombo", &SelectedPort, ports.data());

    PlaceText(BaudRateStr);
    ImGui::Combo("##BaudCombo", &SelectedBaudRate, Serial::Serial::BaudRates.data());

    PlaceText(DataBitsStr);
    ImGui::Combo("##DataCombo", &SelectedDataBits, "5\0""6\0""7\0""8\0");

    PlaceText(StopBitsStr);
    ImGui::Combo("##StopCombo", &SelectedStopBits, "1\0""2\0");

    PlaceText(ParityStr);
    ImGui::Combo("##ParityCombo", &SelectedParity, "None\0Odd\0Even\0Mark\0Space\0");

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SetCursorPosX(155);
    if (ImGui::Button("Ok", { 138, 35 })) Open = false;
    ImGui::End();
    RenderWindow::SetThemeWindow();
}