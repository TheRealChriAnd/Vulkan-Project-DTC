#pragma once
#include <queue>
#include <set>
#include <mutex>
#include <atomic>

namespace std
{
	class thread;
}

class IExecutable;
class IAsynchronous;

class ThreadManager
{
	friend class Application;

public:
	static void scheduleExecution(IExecutable* target);
	static void addAsynchronousService(IAsynchronous* target);
	static void removeAsynchronousService(IAsynchronous* target);

private:
	static void init();
	static void shutdown();
	static void runQueue();
	static void runSet();

private:
	static std::vector<IExecutable*> m_Queue;
	static std::set<IAsynchronous*> m_Set;
	static std::mutex m_MutexQueue;
	static std::mutex m_MutexSetAdd;
	static std::mutex m_MutexSetRemove;
	static std::atomic_bool m_Running;
	static std::thread* m_ThreadQueue;
	static std::thread* m_ThreadSet;
};

