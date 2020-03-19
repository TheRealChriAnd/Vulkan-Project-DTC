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

	static void count(const std::string& name);

	static void reset();
	static void printResults();

private:
	static std::stack<std::pair<std::string, TIME_POINT>> m_Stack;
	static std::unordered_map<std::string, std::vector<double>> m_Data;
	static std::unordered_map<std::string, int> m_DataCountable;
	static TIME_POINT m_StartTime;
};