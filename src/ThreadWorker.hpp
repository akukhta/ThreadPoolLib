#pragma once
#include <thread>

#ifdef _DEBUG
#include <mutex>
#endif

namespace ThreadPoolLib
{
    class ThreadPool;

    class ThreadWorker
    {
    public:
        ThreadWorker(ThreadPool* pool);

        //ThreadWorker(ThreadWorker&& other) : workerMtx(std::move(other.workerMtx)), workerThread(std::move(other.workerThread)) {}

        ThreadWorker(ThreadWorker&&) = default;
        ThreadWorker(ThreadWorker const&) = delete;

        ~ThreadWorker();

        void run();

#ifdef _DEBUG
        size_t getAmountOfProcessedCounts()
        {
            std::unique_lock lk(*workerMtx);
            return taskProcessed;
        }

        std::thread::id getWorkerId()
        {
            return workerThread.get_id();
        }
#endif

    private:
        ThreadPool* pool;
        std::thread workerThread;

#ifdef _DEBUG
        std::unique_ptr<std::mutex> workerMtx = std::make_unique<std::mutex>();
        size_t taskProcessed = 0;
#endif
    };
}