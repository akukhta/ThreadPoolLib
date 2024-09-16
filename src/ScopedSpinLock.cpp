#include "ScopedSpinLock.h"

ThreadPoolLib::ScopedSpinLock::ScopedSpinLock(SpinLock &sl) : sl(sl)
{
    sl.lock();
}

ThreadPoolLib::ScopedSpinLock::~ScopedSpinLock()
{
    sl.unlock();
}
