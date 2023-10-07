#pragma once
#include "../../BaseCheat.h"
struct CUserCmd_t;

class Triggerbot : public BaseCheat {
public:
	bool IgnoreBuildings;
	bool IgnoreTeammates;
	bool IgnoreDormants;
	bool IgnoreDisguised;
	bool IgnoreCloaked;
	bool IgnoreInvulnerable;
	bool BackstabOnly;

	
	bool ScopedOnly;

	uint32_t Key = 0x02;

	float MaximumDistance = 8192.0f;
	float ScaleFactor = 0.7;
	int Hitgroup;

	Triggerbot();
	CBaseEntity* GetEntityOnCrosshair(Vec3);
	void CreateMove(CUserCmd_t*&) override;
	void RenderMenu() override;

	bool Enabled;
};