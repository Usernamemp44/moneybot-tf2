#pragma once
#include "../CBaseEntity/CBaseEntity.h"

class CItem : public CBaseEntity {
public:
	int GetSize();
	int GetType();
	const char * GetPrettyPrintName();
};