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
#else
            std::packaged_task<std::invoke_result_t<FuncType, Args...>(void)> task(boundFunction);
#endif
            auto taskReturnValue = task.get_future();

            {
                ScopedSpinLock lk(sl);

#ifdef __APPLE__
                scheduledTasks.emplace([task]() { task(); });
#else
                scheduledTasks.emplace([task = std::move(task)]() mutable { task(); });
#endif
            }

            return taskReturnValue;
        }

        void finish();

    private:
        friend class SpinLockThreadWorker;
        std::vector<SpinLockThreadWorker> workerThreads;
        SpinLock sl;
        bool isRunning;
        std::deque<std::function<void()>> scheduledTasks;
    };
}
