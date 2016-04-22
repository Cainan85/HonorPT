#include "Ini.h"

Ini::Ini( const char* FileName )
{
    SecureZeroMemory( &m_File, sizeof( smFile ) );
    SecureZeroMemory( m_Result, sizeof( m_Result ) );

    strcpy_s( m_File.Name, FileName );
    GetCurrentDirectoryA( sizeof( m_File.Path ), m_File.Path );
    sprintf_s( m_File.FullPath, "%s\\%s", m_File.Path, m_File.Name );
}