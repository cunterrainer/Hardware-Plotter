#pragma once
#include <iostream>
#include <ostream>
#include <string>
#include <system_error>
#define Endl std::endl
#define GetWinError() Logger::Error(GetLastError())

struct Logger
{
public:
    static std::string Error(DWORD error)
    {
        return "Error: " + std::to_string(error) + " (" + std::system_category().message(static_cast<int>(error)) + ")";
    }
private:
    const char* const m_LogInfo;
    mutable bool newLine = true;
public:
    inline explicit Logger(const char* info) noexcept : m_LogInfo(info) {}

    inline const Logger& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
    {
        std::cout << *osmanip;
        newLine = true;
        return *this;
    }

    template <class T>
    inline const Logger& operator<<(const T& mess) const noexcept
    {
        if (newLine)
        {
            std::cout << m_LogInfo;
            newLine = false;
        }
        std::cout << mess;
        return *this;
    }
};
inline const Logger Log("[INFO] ");
inline const Logger Err("[ERROR] ");