#include <vector>
#include <string>
#include <limits>
#include <Windows.h>

#include "Serial.h"
#include "Log.h"

namespace Serial
{
    Serial::Serial(std::string portName, int selectedBaudRate)
    {
        //Try to connect to the given port
        portName = "\\\\.\\" + portName;
        m_SerialHandle = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_SerialHandle == INVALID_HANDLE_VALUE)
        {
            m_LastErrorMsg = "Handle was not attached. '" + portName + "' " + GetWinError();
            return;
        }

        //If connected we try to set the comm parameters
        DCB dcbSerialParams;
        if (!GetCommState(m_SerialHandle, &dcbSerialParams))
        {
            m_LastErrorMsg = "Failed to get current serial parameters!";
            return;
        }
        
        //Define serial connection parameters for the arduino board
        dcbSerialParams.BaudRate = BaudRateMap.at(selectedBaudRate);
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        //Setting the DTR to Control_Enable ensures that the Arduino is properly
        //reset upon establishing a connection
        dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

        //Set the parameters and check for their proper application
        if (!SetCommState(m_SerialHandle, &dcbSerialParams))
            m_LastErrorMsg = "Could not set Serial Port parameters";
        else
        {
            m_Connected = true;
            //Flush any remaining characters in the buffers 
            PurgeComm(m_SerialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
            //We wait 2s as the arduino board will be reseting since it resets every time you connect
            Sleep(2000);
            m_StartTime = std::chrono::steady_clock::now();
        }
    }


    Serial::~Serial()
    {
        Disconnect();
    }

    Serial::Serial(Serial&& other) noexcept : m_SerialHandle(other.m_SerialHandle), m_Connected(other.m_Connected), m_LastErrorMsg(other.m_LastErrorMsg), m_StartTime(other.m_StartTime)
    {
        other.m_SerialHandle = nullptr;
        other.m_Connected = false;
        other.m_LastErrorMsg.clear();
        other.m_StartTime = std::chrono::steady_clock::time_point();
    }


    Serial& Serial::operator=(Serial&& other) noexcept
    {
        m_SerialHandle = other.m_SerialHandle;
        m_Connected = other.m_Connected;
        m_LastErrorMsg = other.m_LastErrorMsg;
        m_StartTime = other.m_StartTime;

        other.m_SerialHandle = nullptr;
        other.m_Connected = false;
        other.m_LastErrorMsg.clear();
        other.m_StartTime = std::chrono::steady_clock::time_point();
        return *this;
    }


    void Serial::Disconnect() noexcept
    {
        if (m_Connected)
        {
            m_Connected = false;
            CloseHandle(m_SerialHandle);
            m_LastErrorMsg = std::string();
            m_SerialHandle = nullptr;
            m_StartTime = std::chrono::steady_clock::time_point();
        }
    }


    std::string Serial::ReadData() noexcept
    {
        DWORD lastError;
        COMSTAT status;
        ClearCommError(m_SerialHandle, &lastError, &status); // Get info about the serial port

        //Check if there is something to read
        if (status.cbInQue > 0)
        {
            DWORD bytesRead;
            std::string msg(status.cbInQue + 1, 0);
            if (ReadFile(m_SerialHandle, msg.data(), status.cbInQue, &bytesRead, NULL))
            {
                if (bytesRead != status.cbInQue)
                    Log << "Serial::ReadData() bytesRead doesn't match status.cbInQue. bytesRead: " << bytesRead << " status.cbInQue: " << status.cbInQue << Endl;
                return msg;
            }
            else
            {
                m_LastErrorMsg = "Failed to read from serial connection. " + GetWinError();
                Err << m_LastErrorMsg << Endl;
            }
        }
        return std::string();
    }


    bool Serial::WriteData(const char* buffer, unsigned int nbChar)
    {
        DWORD bytesSend;

        //Try to write the buffer on the Serial port
        if (!WriteFile(m_SerialHandle, (const void*)buffer, nbChar, &bytesSend, 0))
        {
            //In case it don't work get comm error and return false
            DWORD lastError = 0;
            COMSTAT status;
            ClearCommError(m_SerialHandle, &lastError, &status);
            m_LastErrorMsg = GetWinError();
            return false;
        }
        else
            return true;
    }


    std::string PortListener::ExtractDeviceName(const std::string& str) noexcept
    {
        std::string deviceName;
        for (size_t i = str.size() - 1; i != (std::numeric_limits<size_t>::max)(); --i)
        {
            if (str[i] == '\\')
                break;
            else
                deviceName += str[i];
        }
        return deviceName.empty() ? "Unknown" : std::string(deviceName.rbegin(), deviceName.rend());
    }


    std::vector<Port> PortListener::GetPorts() noexcept
    {
        char targetPath[255] = { 0 };
        std::vector<Port> ports;
        for (int i = 0; i < 255; i++) // checking ports from COM0 to COM254 windows has 254 ports at max
        {
            const std::string comStr = "COM" + std::to_string(i);
            DWORD error = QueryDosDeviceA(comStr.c_str(), targetPath, 255);
            if (error == 0)
            {
                error = GetLastError();
                if (error != ERROR_FILE_NOT_FOUND) // Is probably the case for most since we check 255 ports
                    Err << "Failed to query ports. " << Logger::Error(error) << Endl;
            }
            else
                ports.push_back({ comStr, ExtractDeviceName(targetPath) });
        }
        return ports;
    }
}