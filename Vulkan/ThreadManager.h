#pragma once
#include <queue>
#include <set>
#include <mutex>
#include <atomic>
#include <functional>
#include "SpinLock.h"

#ifdef VULKAN
#define SERVICE_THREADS 2
#define TASK_THREADS 1
#else
#define SERVICE_THREADS 1
#define TASK_THREADS 0
#endif

#define TASK_THREADS_PLACEHOLDER (TASK_THREADS + 1)

namespace std
{
	class thread;
}

class IAsynchronous;

class ThreadManager
{
	friend class Application;

public:
	static void scheduleExecution(const std::function<void()>& target);
	static void addAsynchronousService(IAsynchronous* target, int threadIndex);
	static void removeAsynchronousService(IAsynchronous* target, int threadIndex);

private:
	static void init();
	static void shutdown();
	static void runQueue(int index);
	static void runSet(int index);

private:
	static std::vector<std::function<void()>> m_Queue[TASK_THREADS_PLACEHOLDER];
	static std::set<IAsynchronous*> m_Set[SERVICE_THREADS];
	static std::atomic_bool m_Running;
	static std::thread* m_ThreadQueue[TASK_THREADS_PLACEHOLDER];
	static std::thread* m_ThreadSets[SERVICE_THREADS];
	static SpinLock m_SetLock[SERVICE_THREADS];
	static SpinLock m_QueueLock[TASK_THREADS_PLACEHOLDER];
};

