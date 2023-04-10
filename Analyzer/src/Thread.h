#pragma once
#include <mutex>
#include <thread>

#include "Log.h"
#include "ImageWriter.h"

namespace Thread
{
    inline void ThreadWait();

    inline std::thread Thread(ThreadWait);
    inline std::mutex Mutex;
    inline bool Stop = false;
    inline ImageWriter* Instance = nullptr;
    inline void(ImageWriter::*ThreadFunc)() = nullptr;

    // for internal use
    inline void ThreadWait()
    {
        Log << "[Thread] Started worker thread, thread id: " << std::this_thread::get_id() << Endl;
        while (true)
        {
            {
                std::lock_guard lock(Mutex);
                if (ThreadFunc != nullptr)
                {
                    Mutex.unlock();
                    (Instance->*ThreadFunc)();
                    Mutex.lock();
                    ThreadFunc = nullptr;
                    Instance = nullptr;
                }
                else if (Stop)
                    return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    inline void Dispatch(void(ImageWriter::*func)(), ImageWriter* instance)
    {
        std::lock_guard lock(Mutex);
        if (ThreadFunc == nullptr)
        {
            Log << "[Thread] Main thread(" << std::this_thread::get_id() << ") successfully dispatched a task to worker thread" << Endl;
            Instance = instance;
            ThreadFunc = func;
            return;
        }
        Err << "[Thread] Main thread(" << std::this_thread::get_id() << ") failed to dispatch a task to the worker thread due to the worker thread being busy" << Endl;
    }

    inline void Join()
    {
        {
            std::lock_guard lock(Mutex);
            Stop = true;
        }
        Thread.join();
        Log << "[Thread] Worker thread joined successfully" << Endl;
    }
}