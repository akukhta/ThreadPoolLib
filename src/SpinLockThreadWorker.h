#pragma once
#include <thread>

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
        SpinLockThreadPool* pool;
        std::thread workerThread;

#ifdef _DEBUG
        std::unique_ptr<std::mutex> workerMtx = std::make_unique<std::mutex>();
        size_t taskProcessed = 0;
#endif
    };
}