#pragma once
#include <chrono>
#include <cstdint>

class Profiler
{
private:
    static inline std::chrono::steady_clock::time_point StartTime;
    static inline std::chrono::nanoseconds AccumulatedTime;
    static inline uint64_t Counter = 0;
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
public:
    static inline void Start()
    {
        StartTime = std::chrono::steady_clock::now();
    }

    static inline void End()
    {
        const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        AccumulatedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - StartTime);
        ++Counter;
    }

    static inline long double Average(long double nanosecConversion = Conversion::Nanoseconds)
    {
        return long double(AccumulatedTime.count() / Counter)*nanosecConversion;
    }

    static inline uint64_t Count()
    {
        return Counter;
    }

    static inline void Reset()
    {
        Counter = 0;
        AccumulatedTime = std::chrono::nanoseconds::zero();
        StartTime = std::chrono::steady_clock::time_point();
    }
};