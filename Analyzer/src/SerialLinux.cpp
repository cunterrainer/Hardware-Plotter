#ifdef LINUX
#include <string>
#include <vector>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <string_view>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "Log.h"
#include "SerialLinux.h"
#include "RenderWindow.h"

namespace Serial
{
    Serial::Serial(Serial&& other) noexcept : m_SerialPort(other.m_SerialPort), m_Connected(other.m_Connected), m_FirstRead(other.m_FirstRead), m_LastErrorMsg(other.m_LastErrorMsg), m_StartTime(other.m_StartTime)
    {
        other.m_SerialPort = 0;
        other.m_Connected = false;
        other.m_FirstRead = true;
        other.m_LastErrorMsg.clear();
        std::memcpy(m_ReadBuf, other.m_ReadBuf, sizeof(m_ReadBuf));
        std::memset(other.m_ReadBuf, 0, sizeof(other.m_ReadBuf));
        other.m_StartTime = std::chrono::steady_clock::time_point();
    }


    Serial& Serial::operator=(Serial&& other) noexcept
    {
        m_SerialPort = other.m_SerialPort;
        m_Connected = other.m_Connected;
        m_FirstRead = other.m_FirstRead;
        m_LastErrorMsg = other.m_LastErrorMsg;
        m_StartTime = other.m_StartTime;
        std::memcpy(m_ReadBuf, other.m_ReadBuf, sizeof(m_ReadBuf));

        other.m_SerialPort = 0;
        other.m_Connected = false;
        other.m_FirstRead = true;
        other.m_LastErrorMsg.clear();
        std::memset(other.m_ReadBuf, 0, sizeof(other.m_ReadBuf));
        other.m_StartTime = std::chrono::steady_clock::time_point();
        return *this;
    }
    

    bool Serial::Connect(const PortSettings& settings) noexcept
    {
        m_SerialPort = open(settings.Port.c_str(), O_RDONLY);
        if (m_SerialPort < 0)
        {
            m_LastErrorMsg = "[Serial] Failed to open port '" + settings.Port + "' " + GetError();
            return false;
        }
        Log << "[Serial] Successfully opened port '" << settings.Port << "' (" << m_SerialPort << ')' << Endl;

        termios2 tty;
        if(ioctl(m_SerialPort, TCGETS2, &tty) != 0)
        {
            m_LastErrorMsg = "[Serial] Failed to get termios2 " + GetError();
            return false;
        }
        Log << "[Serial] Received termios2" << Endl;

        std::memset(&tty, 0, sizeof(tty));
        tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity
        switch(settings.Parity)
        {
            case 1:
                tty.c_cflag |= PARENB; // enable parity
                tty.c_cflag |= PARODD; // set parity odd
                break;
            case 2:
                tty.c_cflag |= PARENB;
                tty.c_cflag &= ~PARODD; // set parity even
                break;
            case 3:
                tty.c_cflag |= PARENB;
                tty.c_cflag |= CMSPAR;
                tty.c_cflag |= PARODD; // set mark parity
                break;
            case 4:
                tty.c_cflag |= PARENB;
                tty.c_cflag |= CMSPAR;
                tty.c_cflag &= ~PARODD; // set space parity
                break;
            default:
                break;
        }

        tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication
        if(settings.StopBits == 1)
            tty.c_cflag |= CSTOPB; // Use two stop bits

        tty.c_cflag &= ~CSIZE; // Clear all the size bits
        switch(settings.DataBits)
        {
            case 0:
                tty.c_cflag |= CS5; // 5 bits per byte
                break;
            case 1:
                tty.c_cflag |= CS6; // 6 bits per byte
                break;
            case 2:
                tty.c_cflag |= CS7; // 7 bits per byte
                break;
            case 3:
                tty.c_cflag |= CS8; // 8 bits per byte
                break;
            default:
                m_LastErrorMsg = "[Serial]{SelectedDataBits} default case! Not allowed to happen!";
                return false;
        }
        tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

        tty.c_cflag &= ~CBAUD;
        tty.c_cflag |= CBAUDEX;
        tty.c_ispeed = BaudRateMap.at(settings.BaudRate);
        tty.c_ospeed = BaudRateMap.at(settings.BaudRate);

        tty.c_lflag &= ~ICANON; // dont receive data line by line
        tty.c_lflag &= ~ECHO; // Disable echo
        tty.c_lflag &= ~ECHOE; // Disable erasure
        tty.c_lflag &= ~ECHONL; // Disable new-line echo
        tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
        tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

        tty.c_cc[VTIME] = 0;
        tty.c_cc[VMIN] = 0;

        if(ioctl(m_SerialPort, TCSETS2, &tty) != 0)
        {
            m_LastErrorMsg = "[Serial] Failed to set termios2 settings " + GetError();
            return false;
        }
        Log << "[Serial] Successfully set termios2 settings" << Endl;
        m_Connected = true;
        m_StartTime = std::chrono::steady_clock::now();
        return true;
    }


    bool Serial::Disconnect() noexcept
    {
        if (!m_Connected)
            return false;
        if(close(m_SerialPort) != 0)
        {
            m_LastErrorMsg = "[Serial] Failed to close serial port " + GetError();
            Err << m_LastErrorMsg << Endl;
            MsgBoxError(m_LastErrorMsg.c_str());
            return false;
        }
        m_SerialPort = 0;
        m_Connected = false;
        m_FirstRead = true;
        m_LastErrorMsg.clear();
        m_StartTime = std::chrono::steady_clock::time_point();
        std::memset(m_ReadBuf, 0, sizeof(m_ReadBuf));
        Log << "[Serial] Successfully disconnected from last port" << Endl;
        return true;
    }


    std::string_view Serial::ReadData() noexcept
    {
        if(!m_Connected)
            return std::string_view();
        ssize_t bytesRead = read(m_SerialPort, m_ReadBuf, sizeof(m_ReadBuf));
        if (bytesRead < 0)
        {
            Err << "[Serial] Failed to read bytes " << GetError() << Endl;
            return std::string_view();
        }

        if (m_FirstRead)
        {
            std::string_view msgView(m_ReadBuf, bytesRead);
            size_t idx = msgView.find_last_of('\n');

            if (idx != std::string::npos && idx > 1)
            {
                m_FirstRead = false;
                const std::chrono::microseconds elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(m_StartTime - std::chrono::steady_clock::now());
                m_StartTime -= elapsedTime; // it is intentional - although 'elapsedTime' is negative
                if (idx != msgView.size() - 1)
                    return std::string_view(&m_ReadBuf[idx + 1], bytesRead - idx - 1);
            }
            return std::string_view();
        }
        return {m_ReadBuf, (size_t)bytesRead};
    }


    std::vector<Port> PortListener::GetPorts() noexcept 
    {
        const std::filesystem::path p("/dev/");
        if(!std::filesystem::exists(p))
        {
            const std::string errorMsg = "[PortListener] Failed to query ports, folder: '" + p.string() + "' doesn't exist";
            Err << errorMsg << Endl;
            MsgBoxError(errorMsg.c_str());
            return std::vector<Port>();
        }

        std::vector<Port> ports;
        for (const auto& di : std::filesystem::directory_iterator(p)) 
        {
            const std::string path = di.path().generic_string();
            if(path.size() < 11)
                continue;
            const std::string_view tty(&path[5], 6);
            if(tty == "ttyUSB" || tty == "ttyACM")
                ports.push_back({path, std::string()});
        }
        return ports;
    }
}
#endif