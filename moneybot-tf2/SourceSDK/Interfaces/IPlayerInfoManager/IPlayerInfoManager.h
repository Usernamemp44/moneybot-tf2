#pragma once
#include "..\CGlobals\CGlobals.h"
class IPlayerInfoManager {
public:
	virtual void *GetPlayerInfo(void *pEdict) = 0;
	virtual CGlobals *GetGlobalVars() = 0;
	static IPlayerInfoManager* factory();
};

#define GlobalVars() IPlayerInfoManager::factory()->GetGlobalVars()