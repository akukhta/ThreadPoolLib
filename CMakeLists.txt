cmake_minimum_required(VERSION 3.28)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(${PROJECT_NAME}
	include/ThreadPool.hpp
	src/ThreadPool.cpp
	src/ThreadWorker.hpp
	src/ThreadWorker.cpp
	include/IThreadPool.h
	src/SpinLook.h
	include/SpinLockThreadPool.h
	src/SpinLockThreadPool.cpp
		src/SpinLockThreadWorker.h
		src/SpinLockThreadWorker.cpp
		src/ScopedSpinLock.cpp
		src/ScopedSpinLock.h
)

target_include_directories(ThreadPool PUBLIC include/)