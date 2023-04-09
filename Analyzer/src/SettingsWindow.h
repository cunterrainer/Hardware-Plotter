#pragma once
#include <string>
#include <vector>

#include "ImGui/imgui.h"

#include "Serial.h"
#include "RenderWindow.h"

class SettingsWindow
{
public:
    static constexpr float Height = 43.f;
private:
    std::vector<Serial::Port> m_Ports = Serial::PortListener::GetPorts();
    std::string m_PortsString;
    int m_SelectedBaudRate = 0;
    int m_SelectedPort = 0;
    bool m_DebugInfo = false;
    bool m_SaveAllClicked = false;
private:
    inline std::string BuildPortsString() const
    {
        std::string content;
        for (const Serial::Port& port : m_Ports)
            content += port.com + '\n' + port.device + '\0';
        return content;
    }
public:
    explicit SettingsWindow() : m_PortsString(BuildPortsString()) {}

    const std::string& GetSelectedPort() const { return m_Ports[(size_t)m_SelectedPort].com; }
    size_t GetNumOfPorts() const { return m_Ports.size(); }
    int GetSelectedBaudRate() const { return m_SelectedBaudRate; }
    bool DebugInfoSelected() const { return m_DebugInfo; }
    bool& SaveAllClicked() { return m_SaveAllClicked; }

    bool ConnectClicked(float windowWidth, bool connected)
    {
        RenderWindow::PushRedButtonColors(connected);
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowBgAlpha(1);
        ImGui::SetNextWindowSize({ windowWidth, Height });
        ImGui::Begin("##Port selection", nullptr, IMGUI_WINDOW_FLAGS);
        const bool clicked = ImGui::Button(connected ? "Disconnect" : "Connect", {150, 0});
        RenderWindow::PopRedButtonColors();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("##PortCombo", &m_SelectedPort, m_PortsString.c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("baud", &m_SelectedBaudRate, Serial::Serial::BaudRates.data());
        ImGui::SameLine();
        if (connected && ImGui::Button("Save all", { 150, 0 }))
            m_SaveAllClicked = true;
        ImGui::SameLine();
        if (ImGui::Button("Update ports", { 150, 0 }))
        {
            m_Ports = Serial::PortListener::GetPorts();
            m_PortsString = BuildPortsString();
            m_SelectedPort = 0;
        }
        ImGui::SameLine();
        ImGui::Checkbox("Debug info", &m_DebugInfo);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Shows the number of y/x values every graph has.\nI left it in on purpose because it might be useful for some people.\nIt just isn't pleasant to look at.");
        ImGui::SameLine(windowWidth - 250);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        return clicked;
    }
};