#include "Log.h"
#include <Windows.h>
#include <mutex>

std::ofstream Log::File = std::ofstream("Log.txt", std::ios::beg);
bool Log::Console = false;
std::mutex Mutex;

void Log::Notice(std::string Message)
{
	if (Console == false)
	{
		AllocConsole();
		Console = true;
	}
	Mutex.lock();

	printf("[Notice ] => %s.\n", Message.c_str());

	Mutex.unlock();
}

void Log::Debug(std::string Message)
{
	if (Console == false)
	{
		AllocConsole();
		Console = true;
	}
	Mutex.lock();

	printf("[Debug ] => %s.\n", Message.c_str());

	Mutex.unlock();
}

void Log::Error(std::string Message, int ExitCode)
{
	if (Console == false)
	{
		AllocConsole();
		Console = true;
	}
	Mutex.lock();

	printf("[Error ] => %s.\n", Message.c_str());

	if (ExitCode >= 0)
	{
		Mutex.unlock();
		ExitProcess(ExitCode);
		return;
	}

	Mutex.unlock();
}
