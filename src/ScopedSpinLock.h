#pragma once
#include "SpinLook.h"

namespace ThreadPoolLib
{
    class ScopedSpinLock
    {
    public:
        ScopedSpinLock(SpinLock &sl);
        ~ScopedSpinLock();

        ScopedSpinLock(ScopedSpinLock&&) = delete;
        ScopedSpinLock(ScopedSpinLock const&) = delete;

    private:
        SpinLock& sl;
    };
}