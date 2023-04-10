#pragma once
#include <string>
#include <string_view>

#include "ImGui/imgui.h"

#include "Serial.h"
#include "PortSettings.h"

class PortSetup
{
private:
    static constexpr ImVec2 WindowSize{ 301, 282 };
    static inline const char* const DeviceStr   = "Device";
    static inline const char* const BaudRateStr = "Baud Rate";
    static inline const char* const DataBitsStr = "Data Bits";
    static inline const char* const StopBitsStr = "Stop Bits";
    static inline const char* const ParityStr   = "Parity";
private:
    static inline bool m_Open = false;
private:
    std::vector<Serial::Port> m_Ports = Serial::PortListener::GetPorts();
    std::string m_PortsString;
    PortSettings m_Settings;
    int m_SelectedPort = 0;
private:
    inline void PlaceText(const char* str) const noexcept
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100 - ImGui::CalcTextSize(str).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
        ImGui::Text("%s", str);
        ImGui::SameLine();
    }

    inline std::string BuildPortsString() const noexcept
    {
        std::string content;
        for (const Serial::Port& port : m_Ports)
            content += port.com + '\n' + port.device + '\0';
        return content;
    }
public:
    PortSetup() : m_PortsString(BuildPortsString()) {}

    void Show() noexcept;
    inline int&        SelectedBaudRate()  noexcept { return m_Settings.BaudRate;    }
    inline int&        SelectedPort()      noexcept { return m_SelectedPort;         }
    inline const char* PortsString() const noexcept { return m_PortsString.c_str();  }
    inline const std::vector<Serial::Port>& Ports() const noexcept { return m_Ports; }

    inline const PortSettings& Settings() noexcept 
    {
        if(!m_Ports.empty())
            m_Settings.Port = m_Ports[m_SelectedPort].com;
        return m_Settings;
    }

    inline void UpdatePorts() noexcept 
    {
        m_Ports = Serial::PortListener::GetPorts();
        m_PortsString = BuildPortsString();
        m_SelectedPort = 0;
    }

    static inline bool IsOpen() { return m_Open; }
};