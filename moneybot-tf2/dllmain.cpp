// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <chrono>
#include "Misc/DynamicNetvars.h"
#include "Cheats/Miscellaneous/Console/Console.h"
#include "Hooks/Hooks.h"

//#include "Cheats/Menu/Menu.h"
using namespace std::chrono;

DWORD WINAPI StartThread(PVOID)
{
	milliseconds tStart = duration_cast< milliseconds >( system_clock::now().time_since_epoch() );
	{

		g_NetVars.Initialize();
		Console::puts("Initialized NetVars.");
		
		InitializeHooks();
		Console::puts("Initialized Hooks.");

	}
	milliseconds tEnd = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	int nTimeToRun = (tEnd - tStart).count();

#ifdef NDEBUG
	const char* szMode = "RELEASE";
#else
	const char* szMode = "DEBUG";
#endif

	char szConsoleOutput[128];
	sprintf_s(szConsoleOutput, 128, "moneybot has fully initialized in %d milliseconds in %s mode!", nTimeToRun, szMode);

	Console::puts(szConsoleOutput, 5.0f, Colour(225, 102, 102).dword);
	return NULL;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, PVOID pReserved)
{
	// Micro Optimization. (https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-disablethreadlibrarycalls)
	DisableThreadLibraryCalls(hinstDLL);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// Initialization Thread.
		{
			HANDLE hHandle = CreateThread(NULL, NULL, StartThread, NULL, NULL, NULL);
			if (hHandle == INVALID_HANDLE_VALUE)
				return FALSE;


			CloseHandle(hHandle);
		}
	}
	return TRUE;
}