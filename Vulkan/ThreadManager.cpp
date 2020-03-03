#include "ThreadManager.h"
#include "IAsynchronous.h"
#include <thread>
#include <iostream>

std::vector<std::function<void()>> ThreadManager::m_Queue;
std::set<IAsynchronous*> ThreadManager::m_Set;
std::atomic_bool ThreadManager::m_Running = true;
std::thread* ThreadManager::m_ThreadQueue = nullptr;
std::thread* ThreadManager::m_ThreadSet = nullptr;

SpinLock ThreadManager::m_SetLock;
SpinLock ThreadManager::m_QueueLock;

void ThreadManager::init()
{
	m_ThreadQueue = new std::thread(&ThreadManager::runQueue);
	m_ThreadSet = new std::thread(&ThreadManager::runSet);
}

void ThreadManager::shutdown()
{
	m_Running = false;
	{
		std::lock_guard<SpinLock> lockQueue(m_QueueLock);
		m_Queue = std::vector<std::function<void()>>();
	}
	{
		std::lock_guard<SpinLock> lockSet(m_SetLock);
		m_Set.clear();
	}
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

		std::vector<std::function<void()>> queue;
		{
			std::lock_guard<SpinLock> lock(m_QueueLock);
			queue = m_Queue;
			m_Queue = std::vector<std::function<void()>>();
		}

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

		std::lock_guard<SpinLock> lock(m_SetLock);
		for (IAsynchronous* target : m_Set)
		{
			target->updateAsynchronous(delta);
		}
	}
}

void ThreadManager::scheduleExecution(const std::function<void()>& target)
{
	std::lock_guard<SpinLock> lock(m_QueueLock);
	m_Queue.push_back(target);
}

void ThreadManager::addAsynchronousService(IAsynchronous* target)
{
	std::lock_guard<SpinLock> lock(m_SetLock);
	m_Set.insert(target);
}

void ThreadManager::removeAsynchronousService(IAsynchronous* target)
{
	std::lock_guard<SpinLock> lock(m_SetLock);
	m_Set.erase(target);
}
