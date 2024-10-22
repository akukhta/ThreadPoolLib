#pragma once
#include <thread>
#include "SpinLook.h"

#ifdef _DEBUG
#include <mutex>
#endif

namespace ThreadPoolLib
{
    class SpinLockThreadPool;

    class SpinLockThreadWorker
    {
    public:
        SpinLockThreadWorker(SpinLockThreadPool* pool);

        //ThreadWorker(ThreadWorker&& other) : workerMtx(std::move(other.workerMtx)), workerThread(std::move(other.workerThread)) {}

        SpinLockThreadWorker(SpinLockThreadWorker&&) = default;
        SpinLockThreadWorker(SpinLockThreadWorker const&) = delete;

        ~SpinLockThreadWorker();

        void run();
        bool isFree();

#ifdef _DEBUG
        size_t getAmountOfProcessedCounts()
        {
            return taskProcessed;
        }

        std::thread::id getWorkerId()
        {
            return workerThread.get_id();
        }
#endif

    private:
        friend class SpinLockThreadPool;

        SpinLockThreadPool* pool;
        std::thread workerThread;
        bool isBusy = false;
        std::unique_ptr<SpinLock> sl = std::make_unique<SpinLock>();
        
#ifdef _DEBUG
        std::unique_ptr<std::mutex> workerMtx = std::make_unique<std::mutex>();
        size_t taskProcessed = 0;
#endif
    };
}