#include "ThreadWorker.hpp"
#include "../include/ThreadPool.hpp"
#include <iostream>

ThreadPoolLib::ThreadWorker::ThreadWorker(ThreadPool* pool)
    : pool(pool)
{
    workerThread = std::thread(&ThreadWorker::run, this);
}

void ThreadPoolLib::ThreadWorker::run()
{
    while (true)
    {
        {
            std::unique_lock lk(pool->mtx);
            pool->cv.wait(lk, [this]()
                          {return !pool->isRunning.load() || !pool->scheduledTasks.empty();});
            
            if (pool->isRunning.load() == false && pool->scheduledTasks.empty())
            {
                break;
            }
        }
        
        std::move_only_function<void()> task;
        
        {
            std::unique_lock lk(pool->mtx);
            
            task = std::move(pool->scheduledTasks.front());
            pool->scheduledTasks.pop();
        }
        
        task();
    }
}

ThreadPoolLib::ThreadWorker::~ThreadWorker()
{
    workerThread.join();
}



