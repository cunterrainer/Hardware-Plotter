#include <vector>
#include <string>
#include <limits>

#include "Serial.h"
#include "Log.h"

namespace Serial
{
    Serial::Serial(const std::string& portName, int selectedBaudRate)
    {
        //Try to connect to the given port
        m_SerialHandle = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_SerialHandle == INVALID_HANDLE_VALUE)
        {
            Err << "Handle was not attached. '" << portName << "' " << LogWinError() << Endl;
            return;
        }

        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};
        if (!GetCommState(m_SerialHandle, &dcbSerialParams))
        {
            Err << "Failed to get current serial parameters!" << Endl;
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
            Err << "Could not set Serial Port parameters" << Endl;
        else
        {
            m_Connected = true;
            //Flush any remaining characters in the buffers 
            PurgeComm(m_SerialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
            //We wait 2s as the arduino board will be reseting since it resets every time you connect
            Sleep(2000);
        }
    }


    Serial::~Serial()
    {
        if (m_Connected)
        {
            m_Connected = false;
            CloseHandle(m_SerialHandle);
        }
    }


    int Serial::ReadData(char* buffer, unsigned int nbChar)
    {
        //Number of bytes we'll have read
        DWORD bytesRead;
        //Number of bytes we'll really ask to read
        unsigned int toRead;

        //Use the ClearCommError function to get status info on the Serial port
        ClearCommError(m_SerialHandle, &m_LastError, &m_Status);

        //Check if there is something to read
        if (m_Status.cbInQue > 0)
        {
            //If there is we check if there is enough data to read the required number
            //of characters, if not we'll read only the available characters to prevent
            //locking of the application.
            if (m_Status.cbInQue > nbChar)
            {
                toRead = nbChar;
            }
            else
            {
                toRead = m_Status.cbInQue;
            }

            //Try to read the require number of chars, and return the number of read bytes on success
            if (ReadFile(m_SerialHandle, buffer, toRead, &bytesRead, NULL))
            {
                return bytesRead;
            }
            else
            {
                Err << "Failed to read from serial connection. " << LogWinError() << Endl;
            }
        }
        //If nothing has been read, or that an error was detected return 0
        return 0;

    }


    bool Serial::WriteData(const char* buffer, unsigned int nbChar)
    {
        DWORD bytesSend;

        //Try to write the buffer on the Serial port
        if (!WriteFile(m_SerialHandle, (void*)buffer, nbChar, &bytesSend, 0))
        {
            //In case it don't work get comm error and return false
            ClearCommError(m_SerialHandle, &m_LastError, &m_Status);
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