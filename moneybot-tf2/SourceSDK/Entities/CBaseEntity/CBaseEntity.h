#pragma once
#include "../../Misc/Defines.h"
#include "../../../Misc/DynamicNetvars.h"

class CBaseEntity {
public:
	Vec3 GetAbsOrigin();
	Vec3 GetAbsAngles();
	void SetAbsAngles(Vec3 orig);

	int DrawModel(int flags);
	void InvalidateBoneCache();
	void SetVecOrigin(Vec3 & vOrigin);
	Vec3 GetVecOrigin();

	Vec3 GetWorldSpaceCenter();

	int GetHealth();
	int GetMaxHealth();

	bool IsWeapon();
	bool IsPlayer();
	bool IsBuilding();
	bool IsPickup();
	bool IsWearable();
	
	bool IsDormant();

	void GetRenderBounds(Vec3 &vMins, Vec3 &vMaxs);

	int GetTeamNum();
	int GetClassId();
	CClientClass* GetClientClass();

	int GetIndex();
	void SetAbsOrigin(Vec3 orig);
	Vec3 GetCollideableMins();
	Vec3 GetCollideableMaxs();
	DWORD * GetModel();
	int GetHitboxSet();
	int GetOwner();
	float GetSimulationTime();
};