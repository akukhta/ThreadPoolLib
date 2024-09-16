#include "SpinLockThreadWorker.h"
#include "SpinLockThreadPool.h"
#include "ScopedSpinLock.h"
#include <functional>

ThreadPoolLib::SpinLockThreadWorker::SpinLockThreadWorker(SpinLockThreadPool *pool)
    : pool(pool)
{
    workerThread = std::thread(&SpinLockThreadWorker::run, this);
}

ThreadPoolLib::SpinLockThreadWorker::~SpinLockThreadWorker()
{
    workerThread.join();
}

void ThreadPoolLib::SpinLockThreadWorker::run()
{
    while (true)
    {
#ifdef __APPLE__
        std::function<void()> task;
#else
        std::move_only_function<void()> task;
#endif
        while (true)
        {
            ScopedSpinLock lk(pool->sl);

            if (pool->isRunning == false && pool->scheduledTasks.empty())
            {
                return;
            }
            else if (pool->isRunning && pool->scheduledTasks.empty())
            {
                continue;
            }
            else if (pool->isRunning && !pool->scheduledTasks.empty())
            {
                task = std::move(pool->scheduledTasks.front());
                pool->scheduledTasks.pop_front();
                break;
            }
        }

        task();

#ifdef _DEBUG
        {
            ++taskProcessed;
        }
#endif
    }
}


