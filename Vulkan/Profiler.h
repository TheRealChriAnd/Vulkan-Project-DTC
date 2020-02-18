#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include <utility>
#include <chrono>

#define TIME_POINT std::chrono::time_point<std::chrono::steady_clock>

class Profiler
{
private:
	Profiler();

public:
	static void begin(const std::string& name);
	static void end();

	static void printResults();

private:
	static std::stack<std::pair<std::string, TIME_POINT>> m_Stack;
	static std::unordered_map<std::string, std::vector<double>> m_Data;
};