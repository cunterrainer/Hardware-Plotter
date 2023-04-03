#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <Windows.h>

#include "Log.h"

namespace Serial
{
    class Serial
    {
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
        explicit Serial(const char* portName);
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

    std::string ExtractDeviceName(const std::string& str) noexcept;
    std::vector<Port> GetPorts() noexcept;
}