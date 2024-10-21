#include "../include/ThreadPool.hpp"

ThreadPoolLib::ThreadPool::ThreadPool(size_t amountOfThreads)
    : amountOfWorkers(amountOfThreads)
{
    threads.reserve(amountOfWorkers);

    for (size_t i = 0; i < amountOfWorkers; ++i)
    {
        threads.emplace_back(this);
    }
}

ThreadPoolLib::ThreadPool::~ThreadPool()
{
    finish();
}

void ThreadPoolLib::ThreadPool::restart()
{
    if (isPoolRunning())
    {
        finish();
    }

    isRunning = true;

    threads.reserve(amountOfWorkers);

    for (size_t i = 0; i < amountOfWorkers; ++i)
    {
        threads.emplace_back(this);
    }
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

    for (auto &worker: threads)
    {
        if (worker.workerThread.joinable())
        {
            worker.workerThread.join();
        }
    }

    threads.clear();
}

bool ThreadPoolLib::ThreadPool::isPoolRunning()
{
    std::unique_lock lk(mtx);
    return isRunning;
}

