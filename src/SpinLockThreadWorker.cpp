#include "SpinLockThreadWorker.h"
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
    if (workerThread.joinable())
    {
        workerThread.join();
    }
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
            else if (!pool->scheduledTasks.empty())
            {
                task = std::move(pool->scheduledTasks.front());
                pool->scheduledTasks.pop_front();
                break;
            }
        }

        {
            ScopedSpinLock sp(*sl.get());
            isBusy = true;
        }

        task();

        {
            ScopedSpinLock sp(*sl.get());
            isBusy = false;
        }

#ifdef _DEBUG
        {
            ++taskProcessed;
        }
#endif
    }
}

bool ThreadPoolLib::SpinLockThreadWorker::isFree()
{
    ScopedSpinLock sp(*sl);
    return isBusy == false;
}


