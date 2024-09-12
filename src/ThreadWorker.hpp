#pragma once
#include <thread>

namespace ThreadPoolLib
{
    class ThreadPool;

    class ThreadWorker
    {
    public:
        ThreadWorker(ThreadPool* pool);

        ThreadWorker(ThreadWorker&&) = default;
        ThreadWorker(ThreadWorker const&) = default;

        ~ThreadWorker();

        void run();

    private:
        ThreadPool* pool;
        std::thread workerThread;
    };
}