#pragma once

template <typename Derived>
class IThreadPool
{
public:
    template <typename FuncType, typename... Args>
    auto addTask(FuncType&& f, Args... args)
    {
        return static_cast<Derived*>(this)->addTask(std::forward<FuncType>(f), std::forward<Args>(args)...);
    }
};