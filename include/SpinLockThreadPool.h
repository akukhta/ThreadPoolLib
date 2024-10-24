#pragma once
#include <vector>
#include <deque>
#include <functional>
#include <future>
#include "IThreadPool.h"
#include "../src/SpinLook.h"
#include "../src/SpinLockThreadWorker.h"
#include "../src/ScopedSpinLock.h"

namespace ThreadPoolLib
{
    class SpinLockThreadPool : public IThreadPool<SpinLockThreadPool>
    {
    public:
        explicit SpinLockThreadPool(size_t amountOfWorkerThreads);
        ~SpinLockThreadPool();

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
                ScopedSpinLock lk(sl);

#ifdef __APPLE__
                scheduledTasks.emplace_back([task]() { (*task)(); });
#else
                scheduledTasks.emplace_back([task = std::move(task)]() mutable { task(); });
#endif
            }

            return taskReturnValue;
        }

        void finish();
        void restart();
        bool isPoolRunning();

        size_t getFreeThreadsCount();

    private:
        friend class SpinLockThreadWorker;
        std::vector<SpinLockThreadWorker> workerThreads;
        SpinLock sl;
        bool isRunning = true;
        size_t amountOfWorkers;

#ifdef  __cpp_lib_move_only_function
        std::deque<std::move_only_function<void()>> scheduledTasks;
#else
        std::deque<std::function<void()>> scheduledTasks;
#endif

    };
}
