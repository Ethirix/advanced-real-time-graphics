#pragma once
#include <cstdarg>
#include <cstdio>
#include <intsafe.h>
#include <windows.h>

constexpr UINT32 MAX_CHARS = 1024;

class Debug
{
public:
	static int DebugPrintF(const char* format, ...)
	{
		va_list argList;
		va_start(argList, format);

		int charsWritten = VDebugPrintF(format, argList);
		va_end(argList);

		return charsWritten;
	}

private:
	static int VDebugPrintF(const char* format, va_list args)
	{
		static char buffer[MAX_CHARS];

		int charsWritten = vsnprintf(buffer, MAX_CHARS, format, args);
		OutputDebugStringA(buffer);

		return charsWritten;
	}
};
