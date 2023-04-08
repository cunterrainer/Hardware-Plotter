#pragma once
#include <mutex>
#include <thread>

#include "Log.h"

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
        Log << "Thread started, thread id: " << std::this_thread::get_id() << Endl;
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
        if (ThreadFunc == nullptr)
        {
            Log << "Dispatched function to thread" << Endl;
            ThreadFunc = func;
            return;
        }
        Log << "Failed to dispatch a function to the thread due to the thread being busy" << Endl;
    }

    inline void Join()
    {
        {
            std::lock_guard lock(Mutex);
            Stop = true;
        }
        Thread.join();
        Log << "Thread joined successfully" << Endl;
    }
}