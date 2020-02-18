#include "Profiler.h"
#include <iostream>
#include <iomanip>

std::stack<std::pair<std::string, TIME_POINT>> Profiler::m_Stack;
std::unordered_map<std::string, std::vector<double>> Profiler::m_Data;

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

void Profiler::printResults()
{
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
	m_Data.clear();
	m_Stack = std::stack<std::pair<std::string, TIME_POINT>>();
	std::cout << std::endl;
}
