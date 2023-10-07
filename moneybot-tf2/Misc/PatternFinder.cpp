#include "PatternFinder.h"
#define INRANGE(x,a,b)	(x >= a && x <= b) 
#define GetBits(x)		(INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define GetByte(x)		(GetBits(x[0]) << 4 | GetBits(x[1]))

UINT64 FindPattern(UINT64 addr, DWORD len, const char* pattern) {
	const char* _pattern = pattern;
	UINT64 firstMatch = 0;
	for (UINT64 current = addr; current < addr + len; current++) {
		if (!*_pattern) {
			return firstMatch;
		}
		/*if (current == 0x261D8A88) {
			Sleep(10000);
		}*/
		if (*(unsigned char*)_pattern == '\?' || *(unsigned char*)current == GetByte(_pattern)) {
			if (!firstMatch)
				firstMatch = current;

			if (!_pattern[2])
				return firstMatch;

			if (*(PWORD)_pattern == '\?\?' || *(PBYTE)_pattern != '\?')
				_pattern += 3;

			else _pattern += 2;
		}
		else
		{
			_pattern = pattern;
			firstMatch = 0;
		}


	}
	return 0;
}

UINT64 FindPatternData(UINT64 addr, DWORD len, const char* pattern, int length = 0) {
	const char* _pattern = pattern;
	UINT64 firstMatch = 0;
	for (UINT64 current = addr; current < len; current++) {
		if (memcmp((void*)current, pattern, length == 0 ? strlen(pattern) : length) == 0)
			return current;
	}
	return 0;
}

HMODULE GetModuleHandleSafely(const char* moduleName)
{
	HMODULE moduleHandle = 0;

	while (true) {
		moduleHandle = GetModuleHandleA(moduleName);
		if (moduleHandle != 0) {
			break;
		}
		Sleep(1);
	}
	return moduleHandle;
}

UINT64 FindPatternInModule(const char* moduleName, const char* pattern) {
	HMODULE module = GetModuleHandleSafely(moduleName);
	PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)module;
#ifdef _WIN64
	PIMAGE_NT_HEADERS64 NTHeaders = (PIMAGE_NT_HEADERS64)(((UINT64)module) + DOSHeader->e_lfanew);

	return FindPattern(((UINT64)module) + NTHeaders->OptionalHeader.BaseOfCode, ((UINT64)module) + NTHeaders->OptionalHeader.SizeOfCode, pattern);
#else
	PIMAGE_NT_HEADERS NTHeaders = (PIMAGE_NT_HEADERS)(((UINT64)module) + DOSHeader->e_lfanew);

	return FindPattern(((UINT64)module) + NTHeaders->OptionalHeader.BaseOfCode, ((UINT64)module) + NTHeaders->OptionalHeader.BaseOfCode + NTHeaders->OptionalHeader.SizeOfCode, pattern);
#endif
}
UINT64 FindPatternInModuleCode(const char* moduleName, const char* pattern, int len = 0) {
	HMODULE module = GetModuleHandleSafely(moduleName);
	PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)module;
#ifdef _WIN64
	PIMAGE_NT_HEADERS64 NTHeaders = (PIMAGE_NT_HEADERS64)(((UINT64)module) + DOSHeader->e_lfanew);
#else
	PIMAGE_NT_HEADERS NTHeaders = (PIMAGE_NT_HEADERS)(((UINT64)module) + DOSHeader->e_lfanew);
#endif
	return FindPatternData(((UINT64)module) + NTHeaders->OptionalHeader.BaseOfCode, ((UINT64)module) + NTHeaders->OptionalHeader.BaseOfCode + NTHeaders->OptionalHeader.SizeOfCode, pattern, len);

}
UINT64 FindPatternInModuleData(const char* moduleName, const char* pattern) {
	HMODULE module = GetModuleHandleSafely(moduleName);
	PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)module;
#ifdef _WIN64
	PIMAGE_NT_HEADERS64 NTHeaders = (PIMAGE_NT_HEADERS64)(((UINT64)module) + DOSHeader->e_lfanew);

	return FindPattern(((UINT64)module) + NTHeaders->OptionalHeader.BaseOfCode, ((UINT64)module) + NTHeaders->OptionalHeader.SizeOfCode, pattern);
#else
	PIMAGE_NT_HEADERS NTHeaders = (PIMAGE_NT_HEADERS)(((UINT64)module) + DOSHeader->e_lfanew);

	return FindPatternData(((UINT64)module) + NTHeaders->OptionalHeader.BaseOfData, ((UINT64)module)  + NTHeaders->OptionalHeader.BaseOfData + NTHeaders->OptionalHeader.SizeOfInitializedData + NTHeaders->OptionalHeader.SizeOfUninitializedData, pattern);
#endif
}

void* CrawlToFunctionPrologue(void* function) {
#ifdef _WIN64
	return -1;
#else
	UINT64 b = (UINT64)function;

	BYTE prologue[] = { 0x55, 0x89, 0xE5 }; //push EBP; c

	while (true) { //53 8B D9
		if (*(unsigned char*)(b) == 0x55 || *(unsigned char*)(b) == 0x53)
			if (*(unsigned char*)(b + 1) == 0x8B)
				if (*(unsigned char*)(b + 2) == 0xEC || *(unsigned char*)(b) == 0xD9)
					break;
		b--;
	}
	return (void*)b;
#endif
}

void* SearchForStringXref(const char* moduleName, const char* string) {
	void* rdata_address = (void*)FindPatternInModuleData(moduleName, string);
	void* real_address = (void*)FindPatternInModuleCode(moduleName, (const char*)(&rdata_address), sizeof(void*));
	void* function_prologue = CrawlToFunctionPrologue(real_address);
	return function_prologue;
}
