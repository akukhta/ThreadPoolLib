#include "../include/SpinLockThreadPool.h"

ThreadPoolLib::SpinLockThreadPool::SpinLockThreadPool(size_t amountOfWorkerThreads)
{
    workerThreads.reserve(amountOfWorkerThreads);

    for (size_t i = 0; i < amountOfWorkerThreads; ++i)
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
    ScopedSpinLock lk(sl);
    isRunning = false;
}
