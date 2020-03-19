#include "Profiler.h"
#include <iostream>
#include <iomanip>

std::stack<std::pair<std::string, TIME_POINT>> Profiler::m_Stack;
std::unordered_map<std::string, std::vector<double>> Profiler::m_Data;
std::unordered_map<std::string, int> Profiler::m_DataCountable;
TIME_POINT Profiler::m_StartTime;

void Profiler::begin(const std::string& name)
{
	m_Stack.push({ name, std::chrono::high_resolution_clock::now() });
}

void Profiler::end()
{
	const std::pair<std::string, TIME_POINT>& pair = m_Stack.top();
	double delta = std::chrono::duration<double, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - pair.second).count();

	auto iterator = m_Data.find(pair.first);
	if (iterator != m_Data.end())
		iterator->second.push_back(delta);
	else
		m_Data.insert({pair.first, {delta}});

	m_Stack.pop();
}

void Profiler::count(const std::string& name)
{
	auto iterator = m_DataCountable.find(name);
	if (iterator != m_DataCountable.end())
		iterator->second++;
	else
		m_DataCountable.insert({ name, 0 });
}

void Profiler::reset()
{
	m_Data.clear();
	m_DataCountable.clear();
	m_StartTime = std::chrono::high_resolution_clock::now();
}

void Profiler::printResults()
{
	double delta = std::chrono::duration<double, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
	for (auto data : m_Data)
	{
		const std::vector<double>& deltas = data.second;
		double average = 0;
		for (auto delta : deltas)
		{
			average += delta;
		}
		average /= deltas.size();
		std::cout << "[" << std::setw(15) << std::left << data.first << "] " << average << std::endl;
	}
	for (auto data : m_DataCountable)
	{
		std::cout << "[" << std::setw(15) << std::left << data.first << "] " << data.second << std::endl;
	}
	std::cout << "[" << std::setw(15) << std::left << "Duration" << "] " << delta << std::endl;
	reset();
	//m_Stack = std::stack<std::pair<std::string, TIME_POINT>>();
	std::cout << std::endl;
}
