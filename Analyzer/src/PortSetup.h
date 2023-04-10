#pragma once
#include <string_view>

#include "ImGui/imgui.h"

#include "Serial.h"

class PortSetup
{
private:
    static inline const char* const DeviceStr   = "Device";
    static inline const char* const BaudRateStr = "Baud Rate";
    static inline const char* const DataBitsStr = "Data Bits";
    static inline const char* const StopBitsStr = "Stop Bits";
    static inline const char* const ParityStr   = "Parity";
public:
    static inline int SelectedPort     = 0;
    static inline int SelectedBaudRate = 6;
    static inline int SelectedDataBits = 3;
    static inline int SelectedStopBits = 0;
    static inline int SelectedParity   = 0;
private:
    static inline bool m_Open = false;
private:
    static inline void PlaceText(const char* str)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100 - ImGui::CalcTextSize(str).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
        ImGui::Text("%s", str);
        ImGui::SameLine();
    }
public:
    static void Show(std::string_view ports);
    static inline bool IsOpen() { return m_Open; }
};