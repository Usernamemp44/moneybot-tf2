#pragma once
#include "../../Misc/Defines.h"
class CBaseEntity;
class CBasePlayer;

class CEntityList
{
public:
	CBaseEntity *GetClientEntity(int nEntityIndex);
	CBasePlayer *GetClientPlayer(int nEntityIndex);
	CBaseEntity *GetClientEntityFromHandle(uintptr_t hEntity);
	CBaseEntity * GetClientEntityFromHandle(int hEntity);
	int GetHighestEntityIndex(void);
	static CEntityList* factory();
	static CEntityList factory_noptr();
};



