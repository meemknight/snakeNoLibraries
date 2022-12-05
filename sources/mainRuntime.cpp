#define NOMINMAX
#include <Windows.h>

#include "types.h"


int main();
void initGlobalConsole();
void setRandomSeed();


void __cdecl
WinMainCRTStartup(void)
{
	
	
	//initGlobalConsole();
	setRandomSeed();


	int exitCode = main();

	
	ExitProcess(exitCode);



}






