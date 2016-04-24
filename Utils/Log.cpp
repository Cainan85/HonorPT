#include "Log.h"
#include <Windows.h>
#include <mutex>

std::ofstream Log::File = std::ofstream("Log.txt", std::ios::beg);
FILE* Log::Console = nullptr;
std::mutex Mutex;

void Log::Notice(std::string Message)
{
	if (Console == nullptr)
	{
		AllocConsole();
		freopen_s(&Console,"CONOUT$", "w", stdout);
	}
	Mutex.lock();

	printf("[Notice] => %s.\n", Message.c_str());
	File << "[Notice] => " << Message << std::endl;

	Mutex.unlock();
}

void Log::Debug(std::string Message)
{
	if (Console == nullptr)
	{
		AllocConsole();
		freopen_s(&Console, "CONOUT$", "w", stdout);
	}
	Mutex.lock();

	printf("[Debug ] => %s.\n", Message.c_str());
	File << "[Debug ] => " << Message << std::endl;

	Mutex.unlock();
}

void Log::Error(std::string Message, int ExitCode)
{
	if (Console == nullptr)
	{
		AllocConsole();
		freopen_s(&Console, "CONOUT$", "w", stdout);
	}
	Mutex.lock();

	printf("[Error ] => %s.\n", Message.c_str());
	File << "[Error ] => " << Message << std::endl;

	if (ExitCode >= 0)
	{
		Mutex.unlock();
		ExitProcess(ExitCode);
		return;
	}

	Mutex.unlock();
}
