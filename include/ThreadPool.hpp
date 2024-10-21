#pragma once
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <queue>
#include <condition_variable>
#include <future>
#include <type_traits>
#include "IThreadPool.h"
#ifdef __APPLE__
#include <memory>
#endif

#include "../src/ThreadWorker.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

namespace ThreadPoolLib
{
    class ThreadPool : public IThreadPool<ThreadPool>
    {
    public:
        ThreadPool(size_t amountOfThreads);
        ~ThreadPool();

        template <typename FuncType, typename... Args>
        auto addTask(FuncType&& f, Args... args)
        {
            auto boundFunction = std::bind(std::forward<FuncType>(f), std::forward<Args>(args)...);

#ifdef __APPLE__
            auto task = std::make_shared<std::packaged_task<std::invoke_result_t<FuncType, Args...>(void)>>(boundFunction);
            auto taskReturnValue = task->get_future();
#else
            std::packaged_task<std::invoke_result_t<FuncType, Args...>(void)> task(boundFunction);
            auto taskReturnValue = task.get_future();
#endif
            {
                std::unique_lock lk(mtx);

#ifdef __APPLE__
                scheduledTasks.emplace_back([task]() { (*task.get())(); });
#else
                scheduledTasks.emplace_back([task = std::move(task)]() mutable { task(); });
#endif

                cv.notify_all();
            }

            return taskReturnValue;
        }

        size_t getFreeThreadsCount();

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

        std::condition_variable cv;

#ifdef  __cpp_lib_move_only_function
        std::deque<std::move_only_function<void()>> scheduledTasks;
#else
        std::deque<std::function<void()>> scheduledTasks;
#endif

        bool isRunning = true;
        std::mutex mtx;
        std::vector<ThreadWorker> threads;
    };
}