#pragma once
#include <string>
#include <fstream>

class Log
{
public:
	Log();
	~Log();

	static void Notice(std::string Message);
	static void Debug(std::string Message);
	static void Error(std::string Message, int ExitCode = -1);

	std::ofstream File;
};

