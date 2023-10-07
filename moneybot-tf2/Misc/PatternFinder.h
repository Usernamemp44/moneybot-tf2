#pragma once
#include <Windows.h>
UINT64 FindPattern(UINT64 addr, DWORD len, const char* pattern);
UINT64 FindPatternInModule(const char* moduleName, const char* pattern);
#define FindPatternInClient(x) (FindPatternInModule("client.dll", x))
#define FindPatternInEngine(x) (FindPatternInModule("engine.dll", x))
void* SearchForStringXref(const char*, const char*);
void* CrawlToFunctionPrologue(void* function);