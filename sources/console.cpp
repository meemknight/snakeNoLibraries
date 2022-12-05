#include <Windows.h>
#include <stringManipulation.h>

HANDLE consoleOut = 0;

void initGlobalConsole()
{
	AllocConsole();
	consoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
}

void consoleWrite(const char *c)
{
	if (!consoleOut) { return; }

	DWORD written = 0;
	WriteConsole(consoleOut, c, stringManipulation::strlen(c), &written, NULL);
}