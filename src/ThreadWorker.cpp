#include "ThreadWorker.hpp"
#include "../include/ThreadPool.hpp"
#include <iostream>

ThreadPoolLib::ThreadWorker::ThreadWorker(ThreadPool* pool)
    : pool(pool)
{
    //std::cout << "Thread Worker constructor" << std::endl;
    workerThread = std::thread(&ThreadWorker::run, this);
}

void ThreadPoolLib::ThreadWorker::run()
{
    while (true)
    {
#ifdef __APPLE__
        std::function<void()> task;
#else
        std::move_only_function<void()> task;
#endif
        {
            std::unique_lock lk(pool->mtx);
            pool->cv.wait(lk, [this]()
                { return !pool->isRunning || !pool->scheduledTasks.empty(); });

            if (pool->isRunning == false && pool->scheduledTasks.empty())
            {
                break;
            }

            task = std::move(pool->scheduledTasks.front());
            pool->scheduledTasks.pop_front();
        }

        {
            std::unique_lock lk(*workerMtx);
            isBusy = true;
        }

        task();

        {
            std::unique_lock lk(*workerMtx);
            isBusy = false;
        }

#ifdef _DEBUG
        {
            std::unique_lock lk(*workerMtx);
            ++taskProcessed;
        }
#endif
    }

    //std::cout << "Thread worker is done" << std::endl;
}

bool ThreadPoolLib::ThreadWorker::isFree()
{
    std::unique_lock lk(*workerMtx);
    return isBusy == false;
}

ThreadPoolLib::ThreadWorker::~ThreadWorker()
{
    if (workerThread.joinable())
    {
        workerThread.join();
    }
    //std::cout << "Thread Worker destructor" << std::endl;
}



