#if defined(LINUX) && !defined(POSIX_COMPLIANT)
#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <string_view>
#include <unordered_map>
#include <asm/termbits.h>

#include "Log.h"
#include "PortSettings.h"

namespace Serial
{
    class Serial
    {
    public:
        static constexpr std::string_view BaudRates = "0\0""50\0""75\0""110\0""134\0""150\0""200\0""300\0""600\0""750\0""1200\0""2400\0""4800\0""9600\0""19200\0""31250\0""38400\0""57600\0""74880\0""115200\0""230400\0""250000\0""460800\0""500000\0""921600\0""1000000\0""2000000\0";
        static inline const std::unordered_map<int, speed_t> BaudRateMap
        {
            {0,  0},
            {1,  50},
            {2,  75},
            {3,  110},
            {4,  134},
            {5,  150},
            {6,  200},
            {7,  300},
            {8,  600},
            {9,  750},
            {10, 1200,},
            {11, 2400,},
            {12, 4800,},
            {13, 9600,},
            {14, 19200},
            {15, 31250},
            {16, 38400},
            {17, 57600},
            {18, 74880},
            {19, 115200},
            {20, 230400},
            {21, 250000},
            {22, 460800},
            {23, 500000},
            {24, 921600},
            {25, 1000000},
            {26, 2000000}
        };
    private:
        int m_SerialPort = 0;
        bool m_Connected = false;
        bool m_FirstRead = true;
        char m_ReadBuf[256] = {0};
        std::string m_LastErrorMsg;
        std::chrono::steady_clock::time_point m_StartTime;
    public:
        Serial() = default;
        ~Serial() { Disconnect(); }
        Serial(Serial&& other) noexcept;
        Serial& operator=(Serial&& other) noexcept;
        Serial(const Serial&) = delete;
        Serial& operator=(const Serial&) = delete;

        bool Connect(const PortSettings& settings) noexcept;
        bool Disconnect() noexcept;
        // ReadData has ownership of the string, if you need a copy do it manually
        std::string_view ReadData() noexcept;
        std::string_view GetLastErrorMsg() const noexcept { return m_LastErrorMsg; }
        constexpr bool IsConnected() const noexcept { return m_Connected; }
        double GetTimeSinceStart() const noexcept { return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_StartTime).count()) / 1000000.0; }
    };

    struct Port
    {
        std::string com;
        std::string device;
    };

    struct PortListener
    {
        static std::vector<Port> GetPorts() noexcept;
    };
}
#endif