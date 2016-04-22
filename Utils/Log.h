#pragma once
#include <string>
#include <fstream>

class Log
{
public:
	static void Notice(std::string Message);
	static void Debug(std::string Message);
	static void Error(std::string Message, int ExitCode = -1);
	
private:
	static std::ofstream File;
};

