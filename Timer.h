#pragma once

#include <chrono>

using namespace std::chrono;

class Timer
{
public:
	Timer() noexcept;
	float Mark() noexcept;
	float Peek() const noexcept;
private:
	steady_clock::time_point last;
};
