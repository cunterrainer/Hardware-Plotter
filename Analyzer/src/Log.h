#pragma once
#include <mutex>
#include <string>
#include <ostream>
#include <iostream>
#include <system_error>
#define Endl std::endl

#ifdef WINDOWS
    #include <Windows.h>
    #undef max // windows macros
    #undef min // windows macros
    #define GetWinError() Logger::Error(GetLastError())
#elif defined(LINUX)
    #include <errno.h>
    #include <unistd.h>
    #define GetError() Logger::Error(errno)
#endif

struct Logger
{
public:
    static std::string Error(int error)
    {
        return "Error: " + std::to_string(error) + " (" + std::system_category().message(static_cast<int>(error)) + ")";
    }
private:
    #ifdef WINDOWS
        static constexpr unsigned char OutputColorWhite = 0b0111;
        static constexpr unsigned char OutputColorLightRed = 0b1100;
    #endif
    static inline std::mutex Mutex;
private:
    const char* const m_LogInfo;
    mutable bool m_NewLine = true;
    mutable bool m_IsErr = false;
private:
    inline void SetOutputColor() const noexcept
    {
        #ifdef WINDOWS
            if(m_IsErr)
            {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, OutputColorLightRed);
            }
        #elif defined(LINUX)
            if(m_IsErr && isatty(STDOUT_FILENO))
                std::cout << "\033[1;31m";
        #endif
    }

    inline void ResetOutputColor() const noexcept
    {
        #ifdef WINDOWS
            if(m_IsErr)
            {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, OutputColorWhite);
            }
        #elif defined(LINUX)
            if(m_IsErr && isatty(STDOUT_FILENO))
                std::cout << "\033[0m";
        #endif
    }
public:
    inline explicit Logger(const char* info, bool isErr) noexcept : m_LogInfo(info), m_IsErr(isErr) {}

    inline const Logger& operator<<(std::ostream& (*osmanip)(std::ostream&)) const noexcept
    {
        std::lock_guard lock(Mutex);
        std::cout << *osmanip;
        ResetOutputColor();
        m_NewLine = true;
        return *this;
    }

    template <class T>
    inline const Logger& operator<<(const T& mess) const noexcept
    {
        std::lock_guard lock(Mutex);
        if (m_NewLine)
        {
            SetOutputColor();
            std::cout << m_LogInfo;
            m_NewLine = false;
        }
        std::cout << mess;
        return *this;
    }
};
inline const Logger Log("[INFO] ", false);
inline const Logger Err("[ERROR] ", true);