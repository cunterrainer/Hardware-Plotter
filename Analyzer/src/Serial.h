#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <Windows.h>

namespace Serial
{
    class Serial
    {
    public:
        static constexpr std::string_view BaudRates = "300""\0""600""\0""750""\0""1200""\0""2400""\0""4800""\0""9600""\0""19200""\0""31250""\0""38400""\0""57600""\0""74880""\0""115200""\0""230400""\0""250000""\0""460800""\0""500000""\0""921600""\0""1000000""\0""2000000";
    private:
        //Serial comm handler
        HANDLE m_SerialHandle;
        //Connection status
        bool m_Connected = false;
        //Get various information about the connection
        COMSTAT m_Status;
        //Keep track of last error
        DWORD m_LastError;
    public:
        //Initialize Serial communication with the given COM port
        explicit Serial(const std::string& portName);
        //Close the connection
        ~Serial();
        //Read data in a buffer, if nbChar is greater than the
        //maximum number of bytes available, it will return only the
        //bytes available. The function returns 0 when nothing could
        //be read or an error occured, otherwise the number of bytes actually read.
        int ReadData(char* buffer, unsigned int nbChar);
        //Writes data from a buffer through the Serial connection
        //return true on success.
        bool WriteData(const char* buffer, unsigned int nbChar);
        constexpr bool IsConnected() const noexcept { return m_Connected; }
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