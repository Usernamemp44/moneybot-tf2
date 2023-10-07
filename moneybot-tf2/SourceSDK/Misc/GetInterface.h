#pragma once
#include <Windows.h>
#include <cassert>
inline void* GetInterface(const char* moduleName, const char* interfaceName)
{
	HMODULE module = NULL;

	module = GetModuleHandleA(moduleName);
	assert(module && "Module name changed?");

	typedef void* (*CreateInterface)(const char* name, int* ret);
	CreateInterface createInterface = reinterpret_cast<CreateInterface>(GetProcAddress(module, "CreateInterface"));

	UINT_PTR ptr = NULL;

	ptr = reinterpret_cast<UINT_PTR>(createInterface(interfaceName, NULL));
	assert(ptr && "Interface name out of date?");
	

	return (void*)(ptr);
}