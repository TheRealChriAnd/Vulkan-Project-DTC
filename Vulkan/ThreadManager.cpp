#include "ThreadManager.h"
#include "IAsynchronous.h"
#include <thread>

std::vector<std::function<void()>> ThreadManager::m_Queue;
std::set<IAsynchronous*> ThreadManager::m_Set;
std::atomic_bool ThreadManager::m_Running = true;
std::mutex ThreadManager::m_MutexQueue;
std::mutex ThreadManager::m_MutexSetAdd;
std::mutex ThreadManager::m_MutexSetRemove;
std::thread* ThreadManager::m_ThreadQueue = nullptr;
std::thread* ThreadManager::m_ThreadSet = nullptr;

void ThreadManager::init()
{
	m_ThreadQueue = new std::thread(&ThreadManager::runQueue);
	m_ThreadSet = new std::thread(&ThreadManager::runSet);
}

void ThreadManager::shutdown()
{
	m_MutexQueue.lock();
	m_Queue = std::vector<std::function<void()>>();
	m_MutexQueue.unlock();

	m_MutexSetAdd.lock();
	m_Set.clear();
	m_MutexSetAdd.unlock();

	m_Running = false;
	m_ThreadQueue->join();
	m_ThreadSet->join();
	delete m_ThreadQueue;
	delete m_ThreadSet;
}

void ThreadManager::runQueue()
{
	while (m_Running)
	{
		if (m_Queue.empty())
			continue;

		m_MutexQueue.lock();
		std::vector<std::function<void()>> queue = m_Queue;
		m_Queue = std::vector<std::function<void()>>();
		m_MutexQueue.unlock();

		for (const std::function<void()>& target : queue)
		{
			target();
		}
	}
}

void ThreadManager::runSet()
{
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (m_Running)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;

		if (m_Set.empty())
			continue;

		m_MutexSetAdd.lock();
		std::set<IAsynchronous*> set = m_Set;
		m_MutexSetAdd.unlock();
		m_MutexSetRemove.lock();
		for (IAsynchronous* target : set)
		{
			target->updateAsynchronous(delta);
		}
		m_MutexSetRemove.unlock();
	}
}

void ThreadManager::scheduleExecution(const std::function<void()>& target)
{
	m_MutexQueue.lock();
	m_Queue.push_back(target);
	m_MutexQueue.unlock();
}

void ThreadManager::addAsynchronousService(IAsynchronous* target)
{
	m_MutexSetAdd.lock();
	m_Set.insert(target);
	m_MutexSetAdd.unlock();
}

void ThreadManager::removeAsynchronousService(IAsynchronous * target)
{
	m_MutexSetRemove.lock();
	m_Set.erase(target);
	m_MutexSetRemove.unlock();
}
