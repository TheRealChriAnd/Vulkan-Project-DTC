#include "ThreadManager.h"
#include "IAsynchronous.h"
#include <thread>
#include <iostream>

std::vector<std::function<void()>> ThreadManager::m_Queue[];
std::set<IAsynchronous*> ThreadManager::m_Set[];
std::atomic_bool ThreadManager::m_Running = true;
std::thread* ThreadManager::m_ThreadQueue[];
std::thread* ThreadManager::m_ThreadSets[];

SpinLock ThreadManager::m_SetLock[];
SpinLock ThreadManager::m_QueueLock[];

void ThreadManager::init()
{
	for (int i = 0; i < TASK_THREADS; i++)
		m_ThreadQueue[i] = new std::thread(&ThreadManager::runQueue, i);

	for(int i = 0; i < SERVICE_THREADS; i++)
		m_ThreadSets[i] = new std::thread(&ThreadManager::runSet, i);
}

void ThreadManager::shutdown()
{
	m_Running = false;
	for (int i = 0; i < TASK_THREADS; i++)
	{
		std::lock_guard<SpinLock> lockQueue(m_QueueLock[i]);
		m_Queue[i] = std::vector<std::function<void()>>();
	}
	for (int i = 0; i < SERVICE_THREADS; i++)
	{
		std::lock_guard<SpinLock> lockSet(m_SetLock[i]);
		m_Set[i].clear();
	}

	for (int i = 0; i < TASK_THREADS; i++)
	{
		m_ThreadQueue[i]->join();
		delete m_ThreadQueue[i];
	}
	for (int i = 0; i < SERVICE_THREADS; i++)
	{
		m_ThreadSets[i]->join();
		delete m_ThreadSets[i];
	}
}

void ThreadManager::runQueue(int index)
{
	while (m_Running)
	{
		if (m_Queue[index].empty())
			continue;

		std::vector<std::function<void()>> queue;
		{
			std::lock_guard<SpinLock> lock(m_QueueLock[index]);
			queue = m_Queue[index];
			m_Queue[index] = std::vector<std::function<void()>>();
		}

		for (const std::function<void()>& target : queue)
		{
			target();
		}
	}
}

void ThreadManager::runSet(int index)
{
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (m_Running)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;

		if (m_Set[index].empty())
			continue;

		std::lock_guard<SpinLock> lock(m_SetLock[index]);
		for (IAsynchronous* target : m_Set[index])
		{
			target->updateAsynchronous(delta);
		}
	}
}

void ThreadManager::scheduleExecution(const std::function<void()>& target)
{
	std::lock_guard<SpinLock> lock(m_QueueLock[0]);
	m_Queue[0].push_back(target);
}

void ThreadManager::addAsynchronousService(IAsynchronous* target, int threadIndex)
{
	std::lock_guard<SpinLock> lock(m_SetLock[threadIndex]);
	m_Set[threadIndex].insert(target);
}

void ThreadManager::removeAsynchronousService(IAsynchronous* target, int threadIndex)
{
	std::lock_guard<SpinLock> lock(m_SetLock[threadIndex]);
	m_Set[threadIndex].erase(target);
}
