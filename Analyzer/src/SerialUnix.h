#if defined(UNIX_COMPLIANT) || defined(MAC_OS)
#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <string_view>
#include <unordered_map>
#include <termios.h>

#include "Log.h"
#include "PortSettings.h"

namespace Serial
{
    class Serial
    {
    public:
        static constexpr std::string_view BaudRates = "0\0""50\0""75\0""110\0""134\0""150\0""200\0""300\0""600\0""1200\0""1800\0""2400\0""4800\0""9600\0""19200\0""38400\0""57600\0""115200\0""230400\0";
        static inline const std::unordered_map<int, unsigned int> BaudRateMap
        {
            {0,  B0},
            {1,  B50},
            {2,  B75},
            {3,  B110},
            {4,  B134},
            {5,  B150},
            {6,  B200},
            {7,  B300},
            {8,  B600},
            {9,  B1200},
            {10, B1800},
            {11, B2400},
            {12, B4800},
            {13, B9600},
            {14, B19200},
            {15, B38400},
            {16, B57600},
            {17, B115200},
            {18, B230400}
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

        bool Connect(const PortSettings&) noexcept;
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