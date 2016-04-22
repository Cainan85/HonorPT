#pragma once
#include <Windows.h>
#include <sstream>

// Manipula um arquivo .ini.
class Ini
{
public:
	// Construtor.
	Ini(const char* FileName);

	// Métodos.
	template< typename Template >
	void     Write(const char* Section, const char* Key, Template Value)
	{
		std::stringstream Cast(Value);
		std::string String;
		Cast >> String;
		WritePrivateProfileStringA(Section, Key, String.c_str(), m_File.FullPath);
	}
	template< typename Template >
	Template Read(const char* Section, const char* Key)
	{
		SecureZeroMemory(m_Result, sizeof(m_Result));
		GetPrivateProfileString(Section, Key, 0, m_Result, sizeof(m_Result), m_File.FullPath);
		std::istringstream Cast(m_Result);
		Template Result{};
		Cast >> std::noskipws >> Result;
		return Result;
	}
	template< >
	std::string Read<std::string>(const char* Section, const char* Key)
	{
		SecureZeroMemory(m_Result, sizeof(m_Result));
		GetPrivateProfileString(Section, Key, 0, m_Result, sizeof(m_Result), m_File.FullPath);
		return m_Result;
	}
private:
	// Atributos.
	char m_Result[256];
	struct smFile
	{
		char Name[256];
		char Path[512];
		char FullPath[256 + 512];
	} m_File;
};