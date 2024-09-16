#pragma once
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <functional>
#include <queue>
#include <condition_variable>
#include <future>
#include <type_traits>
#include "../src/ThreadWorker.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

namespace ThreadPoolLib
{
    class ThreadPool
    {
    public:
        ThreadPool(size_t amountOfThreads);

        ~ThreadPool();

        template <typename FuncType, typename... Args>
        auto addTask(FuncType&& f, Args... args)
        {
            auto boundFunction = std::bind(std::forward<FuncType>(f), std::forward<Args>(args)...);

            std::packaged_task<std::invoke_result_t<FuncType, Args...>(void)> task(boundFunction);

            auto taskReturnValue = task.get_future();

            {
                {
                    mtx.lock();
                    //std::unique_lock lk(mtx);
                    scheduledTasks.emplace([task = std::move(task)]() mutable { task(); });
                    mtx.unlock();
                }

                cv.notify_one();
            }

            return taskReturnValue;
        }

        void finish();

#ifdef _DEBUG
        void printInfo()
        {
            for (auto& worker : threads)
            {
                std::cout << worker.getWorkerId() << ":\t" << worker.getAmountOfProcessedCounts() << "\n";
            }
        }
#endif
    private:
        friend class ThreadWorker;

        std::condition_variable_any cv;
        std::queue<std::move_only_function<void()>> scheduledTasks;
        bool isRunning = true;
        std::shared_mutex mtx;
        std::vector<ThreadWorker> threads;
    };
}