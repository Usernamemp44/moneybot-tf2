#pragma once
#include "../CBaseEntity/CBaseEntity.h"
#include "../CBasePlayer/CBasePlayer.h"
class CBuilding : public CBaseEntity{
public:
	CBaseEntity * GetOwner();
	int GetLevel();
	bool IsSapped();
	int GetHealth();
	int GetMaxHealth();
	bool IsCarried();
};