#ifdef WINDOWS
#pragma once
#include <mutex>
#include <string>
#include <ostream>
#include <iostream>
#include <system_error>
#include <Windows.h>
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
    static constexpr WORD OutputColorWhite = 0b0111;
    static constexpr WORD OutputColorLightRed = 0b1100;
    static inline std::mutex Mutex;
private:
    const char* const m_LogInfo;
    mutable bool m_NewLine = true;
    mutable bool m_IsErr = false;
    mutable HANDLE m_Console = GetStdHandle(STD_OUTPUT_HANDLE);
public:
    inline explicit Logger(const char* info, bool isErr) noexcept : m_LogInfo(info), m_IsErr(isErr) {}

    inline const Logger& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
    {
        std::lock_guard lock(Mutex);
        std::cout << *osmanip;
        m_NewLine = true;
        SetConsoleTextAttribute(m_Console, OutputColorWhite);
        return *this;
    }

    template <class T>
    inline const Logger& operator<<(const T& mess) const noexcept
    {
        std::lock_guard lock(Mutex);
        if (m_NewLine)
        {
            if (m_IsErr)
                SetConsoleTextAttribute(m_Console, OutputColorLightRed);
            std::cout << m_LogInfo;
            m_NewLine = false;
        }
        std::cout << mess;
        return *this;
    }
};
inline const Logger Log("[INFO] ", false);
inline const Logger Err("[ERROR] ", true);
#else

#pragma once
#include <mutex>
#include <string>
#include <ostream>
#include <iostream>
#include <system_error>
#define Endl std::endl

struct Logger
{
public:
    static std::string Error(unsigned long error)
    {
        return "Error: " + std::to_string(error) + " (" + std::system_category().message(static_cast<int>(error)) + ")";
    }
private:
    static inline std::mutex Mutex;
private:
    const char* const m_LogInfo;
    mutable bool m_NewLine = true;
    mutable bool m_IsErr = false;
public:
    inline explicit Logger(const char* info, bool isErr) noexcept : m_LogInfo(info), m_IsErr(isErr) {}

    inline const Logger& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
    {
        std::lock_guard lock(Mutex);
        std::cout << *osmanip;
        m_NewLine = true;
        return *this;
    }

    template <class T>
    inline const Logger& operator<<(const T& mess) const noexcept
    {
        std::lock_guard lock(Mutex);
        if (m_NewLine)
        {
            std::cout << m_LogInfo;
            m_NewLine = false;
        }
        std::cout << mess;
        return *this;
    }
};
inline const Logger Log("[INFO] ", false);
inline const Logger Err("[ERROR] ", true);
#endif