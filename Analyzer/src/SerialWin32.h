#ifdef WINDOWS
#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <string_view>
#include <unordered_map>
#include <Windows.h>
#undef max // windows macros
#undef min // windows macros

#include "Log.h"
#include "PortSettings.h"

namespace Serial
{
    class Serial
    {
    public:
        static constexpr std::string_view BaudRates = "0\0""50\0""75\0""110\0""134\0""150\0""200\0""300\0""600\0""750\0""1200\0""2400\0""4800\0""9600\0""19200\0""31250\0""38400\0""57600\0""74880\0""115200\0""230400\0""250000\0""460800\0""500000\0""921600\0""1000000\0""2000000\0";
        static inline const std::unordered_map<int, DWORD> BaudRateMap
        {
            {0,  0},
            {1,  50},
            {2,  75},
            {3,  CBR_110},
            {4,  134},
            {5,  150},
            {6,  200},
            {7,  CBR_300},
            {8,  CBR_600},
            {9,  750},
            {10, CBR_1200,},
            {11, CBR_2400,},
            {12, CBR_4800,},
            {13, CBR_9600,},
            {14, CBR_19200},
            {15, 31250},
            {16, CBR_38400},
            {17, CBR_57600},
            {18, 74880},
            {19, CBR_115200},
            {20, 230400},
            {21, 250000},
            {22, 460800},
            {23, 500000},
            {24, 921600},
            {25, 1000000},
            {26, 2000000}
        };
    private:
        HANDLE m_SerialHandle = nullptr;
        bool m_Connected = false;
        bool m_FirstRead = true;
        std::string m_ReadData = std::string(255, 0);
        std::string m_LastErrorMsg;
        std::chrono::steady_clock::time_point m_StartTime;
    public:
        Serial() = default;
        ~Serial();
        Serial(Serial&& other) noexcept;
        Serial& operator=(Serial&& other) noexcept;
        Serial(const Serial&) = delete;
        Serial& operator=(const Serial&) = delete;

        bool Connect(PortSettings settings) noexcept;
        void Disconnect() noexcept;
        // ReadData has ownership of the string, if you need a copy do it manually
        std::string_view ReadData() noexcept;
        bool WriteData(const char* buffer, unsigned int nbChar);
        constexpr bool IsConnected() const noexcept { return m_Connected; }
        std::string_view GetLastErrorMsg() const noexcept { return m_LastErrorMsg; }
        double GetTimeSinceStart() const { return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_StartTime).count()) / 1000000.0; }
    };

    struct Port
    {
        std::string com;
        std::string device;
    };

    class PortListener
    {
    private:
        static std::string ExtractDeviceName(const std::string& str) noexcept;
    public:
        static std::vector<Port> GetPorts() noexcept;
    };
}
#endif