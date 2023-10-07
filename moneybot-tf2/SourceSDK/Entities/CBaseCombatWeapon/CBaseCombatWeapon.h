#pragma once
#include "../CBaseEntity/CBaseEntity.h"

class CBaseCombatWeapon : public CBaseEntity {
public:

	char* GetName();
	int GetSlot();
	float GetChargeTime();
	float GetChargeDamage();
	int GetItemDefinitionIndex();
	float GetLastFireTime();
	float GetSwingRange(CBaseEntity *pLocal);
	bool DoSwingTrace(CGameTrace &Trace);
	Vec3 &GetBulletSpread();
	int GetDamageType();
	float GetNextPrimaryAttack();
	bool CanFireCriticalShot(bool bHeadShot = false);
	bool IsReadyToFire();
};