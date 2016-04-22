#include "Log.h"
#include <Windows.h>
#include <mutex>

std::ofstream Log::File = std::ofstream("Log.txt", std::ios::beg);
std::mutex Mutex;

void Log::Notice(std::string Message)
{
	Mutex.lock();

	File << "[Notice] => " << Message << std::endl;

	Mutex.unlock();
}

void Log::Debug(std::string Message)
{
	Mutex.lock();

	File << "[Debug ] => " << Message << std::endl;
	Mutex.unlock();
}

void Log::Error(std::string Message, int ExitCode)
{
	Mutex.lock();

	File << "[Error ] => " << Message << std::endl;

	if (ExitCode >= 0)
	{
		Mutex.unlock();
		ExitProcess(ExitCode);
		return;
	}

	Mutex.unlock();
}
