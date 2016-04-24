#include "Log.h"
#include <Windows.h>
#include <mutex>

std::mutex Thread;
Log* Logging = new Log();

Log::Log()
{
	File = std::ofstream("C:\\pstale\\Log.txt", std::ios::beg);
}

Log::~Log()
{
	File.close();
}

void Log::Notice(std::string Message)
{
	Thread.lock();
	Logging->File << "[Notice] => " << Message << std::endl;
	Thread.unlock();
}

void Log::Debug(std::string Message)
{
	Thread.lock();
	Logging->File << "[Debug ] => " << Message << std::endl;
	Thread.unlock();
}

void Log::Error(std::string Message, int ExitCode)
{
	Thread.lock();
	Logging->File << "[Error ] => " << Message << std::endl;

	if (ExitCode != -1)
	{
		Thread.unlock();
		ExitProcess(ExitCode);
	}
	Thread.unlock();
}
