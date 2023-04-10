#include "PortSetup.h"
#include "RenderWindow.h"

void PortSetup::Show() noexcept
{
    m_Open = true;
    RenderWindow::SetThemePopup();

    // // only applies for the first time
    const ImVec2 dpSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos({ (dpSize.x - WindowSize.x) / 2.f, (dpSize.y - WindowSize.y) / 2.f }, ImGuiCond_Once);

    ImGui::Begin("Port Setup", &m_Open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    const ImVec2 pos      = ImGui::GetWindowPos();
    const ImVec2 size     = ImGui::GetWindowSize();
    const ImVec2 mousePos = ImGui::GetMousePos();
    if (mousePos.x < pos.x || mousePos.x > pos.x + size.x || mousePos.y < pos.y || mousePos.y > pos.y + size.y)
        ImGui::SetWindowFocus();

    PlaceText(DeviceStr);
    ImGui::Combo("##DeviceCombo", &m_SelectedPort, m_PortsString.c_str());

    PlaceText(BaudRateStr);
    ImGui::Combo("##BaudCombo", &m_Settings.BaudRate, Serial::Serial::BaudRates.data());

    PlaceText(DataBitsStr);
    ImGui::Combo("##DataCombo", &m_Settings.DataBits, "5\0""6\0""7\0""8\0");

    PlaceText(StopBitsStr);
    ImGui::Combo("##StopCombo", &m_Settings.StopBits, "1\0""2\0");

    PlaceText(ParityStr);
    ImGui::Combo("##ParityCombo", &m_Settings.Parity, "None\0Odd\0Even\0Mark\0Space\0");

    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::NewLine();
    ImGui::SetCursorPosX(155);
    if (ImGui::Button("Ok", { 138, 35 })) m_Open = false;
    ImGui::End();
    RenderWindow::SetThemeWindow();
}