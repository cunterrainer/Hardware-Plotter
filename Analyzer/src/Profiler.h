#pragma once
#include <chrono>
#include <cstdint>
#include <iostream>
#include <unordered_map>

class Profiler
{
public:
    struct Conversion
    {
        static constexpr long double Nanoseconds  = 1.0;
        static constexpr long double Microseconds = 0.001;
        static constexpr long double Milliseconds = 0.000001;
        static constexpr long double Seconds      = 0.000000001;
        static constexpr long double Minutes      = 1.6666666666667E-11;
        static constexpr long double Hours        = 2.777777777E-13;
    };
private:
    static inline std::chrono::steady_clock::time_point StartTime;
    static inline std::chrono::nanoseconds AccumulatedTime = std::chrono::nanoseconds::zero();
    static inline std::uint64_t Counter = 0;
    static inline std::unordered_map<long double, const char*> TimeAbbreviations
    {
        {Conversion::Nanoseconds,  "ns"},
        {Conversion::Microseconds, "mcs"},
        {Conversion::Milliseconds, "ms"},
        {Conversion::Seconds,      "sec"},
        {Conversion::Minutes,      "min"},
        {Conversion::Hours,        "h"},
    };
private:
    static inline bool Log(long double nanosecConversion, const char* name, bool resetOnLog)
    {
        std::cout << "[Profiler] " << name << " Count: " << Count() << " Average: " << Average(nanosecConversion) << ' ' << TimeAbbreviations[nanosecConversion] << '\n';
        if (resetOnLog)
            Profiler::Reset();
        return true;
    }
public:
    static inline void Start()
    {
        StartTime = std::chrono::steady_clock::now();
    }

    static inline void End(bool increaseCounter = true)
    {
        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        AccumulatedTime += std::chrono::duration_cast<std::chrono::nanoseconds>(now - StartTime);
        if(increaseCounter)
            ++Counter;
    }

    static long double Average(long double nanosecConversion = Conversion::Nanoseconds)
    {
        return ((long double)AccumulatedTime.count() / (long double)Counter)*nanosecConversion;
    }

    static inline std::uint64_t Count()
    {
        return Counter;
    }

    static void Reset()
    {
        Counter = 0;
        AccumulatedTime = std::chrono::nanoseconds::zero();
        StartTime = std::chrono::steady_clock::time_point();
    }

    // Log if Count() is equal to 'value'
    static inline bool LogIfEq(std::uint64_t value, long double nanosecConversion = Conversion::Nanoseconds, const char* name = "", bool resetOnLog = true)
    {
        if (Count() == value)
            return Log(nanosecConversion, name, resetOnLog);
        return false;
    }

    // Log if Count() is equal to or greater than 'value'
    static inline bool LogIfEqGr(std::uint64_t value, long double nanosecConversion = Conversion::Nanoseconds, const char* name = "", bool resetOnLog = true)
    {
        if (Count() >= value)
            return Log(nanosecConversion, name, resetOnLog);
        return false;
    }
};