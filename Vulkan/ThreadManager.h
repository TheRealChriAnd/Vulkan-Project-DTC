#pragma once
#include <queue>
#include <set>
#include <mutex>
#include <atomic>
#include <functional>
#include "SpinLock.h"

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
	static void addAsynchronousService(IAsynchronous* target);
	static void removeAsynchronousService(IAsynchronous* target);

private:
	static void init();
	static void shutdown();
	static void runQueue();
	static void runSet();

private:
	static std::vector<std::function<void()>> m_Queue;
	static std::set<IAsynchronous*> m_Set;
	static std::atomic_bool m_Running;
	static std::thread* m_ThreadQueue;
	static std::thread* m_ThreadSet;
	static SpinLock m_SetLock;
	static SpinLock m_QueueLock;
};

