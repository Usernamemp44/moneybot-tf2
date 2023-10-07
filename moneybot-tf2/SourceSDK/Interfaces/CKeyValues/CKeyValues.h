#pragma once
#include "../../Misc/Defines.h"
#include "../../../Misc/PatternFinder.h"
class CKeyValues
{
public:
	static bool LoadFromBuffer(KeyValues *key_value, char const *resource_name, const char *buffer, IFileSystem *file_system = 0, const char *path_id = 0);
	static KeyValues *Initialize(KeyValues *key_value, char *name);
};