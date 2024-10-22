#include "SpinLockThreadPool.h"
#include "SpinLockThreadPool.h"
#include "SpinLockThreadPool.h"
#include "../include/SpinLockThreadPool.h"

ThreadPoolLib::SpinLockThreadPool::SpinLockThreadPool(size_t amountOfWorkerThreads) 
    : amountOfWorkers(amountOfWorkerThreads)
{
    workerThreads.reserve(amountOfWorkers);

    for (size_t i = 0; i < amountOfWorkers; ++i)
    {
        workerThreads.emplace_back(this);
    }
}

ThreadPoolLib::SpinLockThreadPool::~SpinLockThreadPool()
{
    finish();
}

void ThreadPoolLib::SpinLockThreadPool::finish()
{
    {
        ScopedSpinLock lk(sl);
        isRunning = false;
    }

    for (auto& worker : workerThreads)
    {
        if (worker.workerThread.joinable())
        {
            worker.workerThread.join();
        }
    }

    workerThreads.clear();
}

void ThreadPoolLib::SpinLockThreadPool::restart()
{
    if (isPoolRunning())
    {
        finish();
    }

    isRunning = true;
    workerThreads.reserve(amountOfWorkers);

    for (size_t i = 0; i < amountOfWorkers; ++i)
    {
        workerThreads.emplace_back(this);
    }
}

bool ThreadPoolLib::SpinLockThreadPool::isPoolRunning()
{
    ScopedSpinLock lk(sl);
    return isRunning;
}

size_t ThreadPoolLib::SpinLockThreadPool::getFreeThreadsCount()
{
    size_t count = 0;

    for (auto& worker : workerThreads)
    {
        count += worker.isFree() ? 1 : 0;
    }

    return count;
}
