
#pragma once

#include <chrono>
#include <ctime>
#include <string>

class Chrono
{
public:

	typedef std::chrono::milliseconds	milliseconds;
	typedef std::chrono::seconds		seconds;

	std::chrono::time_point<std::chrono::system_clock> start, end;

	void Start()
	{
		start = std::chrono::system_clock::now();
	}

	void End()
	{
		end = std::chrono::system_clock::now();
	}

	template<typename chronotype = Chrono::milliseconds>
	int Count()
	{
		int elapsed = std::chrono::duration_cast<chronotype>
			(end - start).count();
		return elapsed;
	}

};
