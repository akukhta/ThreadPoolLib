#include "../include/ThreadPool.hpp"

ThreadPoolLib::ThreadPool::ThreadPool(size_t amountOfThreads)
//    : threads(amountOfThreads, ThreadWorker(this))
{
    threads.reserve(amountOfThreads);

    for (size_t i = 0; i < amountOfThreads; ++i)
    {
        threads.emplace_back(this);
    }
}

ThreadPoolLib::ThreadPool::~ThreadPool()
{
    finish();
}


size_t ThreadPoolLib::ThreadPool::getFreeThreadsCount()
{
    size_t count = 0;

    for (auto &worker: threads)
    {
        count += worker.isFree() ? 1 : 0;
    }

    return count;
}

void ThreadPoolLib::ThreadPool::finish()
{
    {
        std::unique_lock lk(mtx);
        isRunning = false;
    }

    cv.notify_all();
}
