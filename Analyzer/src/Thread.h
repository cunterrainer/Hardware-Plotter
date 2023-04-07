#pragma once
#include <mutex>
#include <thread>
#include <iostream>

namespace Thread
{
    inline void ThreadWait();

    inline std::thread Thread(ThreadWait);
    inline std::mutex Mutex;
    inline bool Stop = false;
    inline void(*ThreadFunc)() = nullptr;

    // for internal use
    inline void ThreadWait()
    {
        while (true)
        {
            {
                std::lock_guard lock(Mutex);
                if (ThreadFunc != nullptr)
                {
                    Mutex.unlock();
                    ThreadFunc();
                    Mutex.lock();
                    ThreadFunc = nullptr;
                }
                else if (Stop)
                    return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    inline void Dispatch(void(*func)())
    {
        std::lock_guard lock(Mutex);
        if(ThreadFunc == nullptr)
            ThreadFunc = func;
    }

    inline void Join()
    {
        {
            std::lock_guard lock(Mutex);
            Stop = true;
        }
        Thread.join();
    }
}