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


void ThreadPoolLib::ThreadPool::finish()
{
    {
        std::unique_lock lk(mtx);
        isRunning = false;
    }

    cv.notify_all();
}
