#include "../include/ThreadPool.hpp"

ThreadPoolLib::ThreadPool::ThreadPool(size_t amountOfThreads)
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
    isRunning.store(false);
    cv.notify_all();
}
