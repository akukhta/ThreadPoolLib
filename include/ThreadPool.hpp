#pragma once
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <queue>
#include <condition_variable>
#include <future>
#include <type_traits>
#include "../src/ThreadWorker.hpp"

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
                std::unique_lock lk(mtx);
                scheduledTasks.emplace([task = std::move(task)]() mutable { task(); });
                cv.notify_all();
            }

            return taskReturnValue;
        }

        void finish();

    private:
        friend class ThreadWorker;

        std::condition_variable cv;
        std::queue<std::move_only_function<void()>> scheduledTasks;
        bool isRunning = true;
        std::mutex mtx;
        std::vector<ThreadWorker> threads;
    };
}