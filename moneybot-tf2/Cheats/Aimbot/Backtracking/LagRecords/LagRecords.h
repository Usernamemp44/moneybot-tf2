#pragma once

#include "../../../BaseCheat.h"
#include "../../../../SourceSDK/Misc/Vector3.h"
#include "../../../../SourceSDK/Misc/Defines.h"
#include "../../../../SourceSDK/Interfaces/ConVar/ConVar.h"

#include <deque>
#include <mutex>

class CBasePlayer;

struct LagRecord {
	int tickcount;
	float simtime;
	Vec3 eyeangles;
	Vec3 absangles;
	matrix3x4 bonematrix[256];
	Vec3 absorigin;
	Vec3 vecorigin;
	Vec3 velocity;
	int chokedticks;
	bool was_visible = false;
	int hitbox;
};

class LagRecords : public BaseCheat {
	ConVar* InterpRatio;
	ConVar* Interp;
	ConVar* MinInterpRatio;
	ConVar* MaxInterpRatio;
	ConVar* UpdateRate;
	ConVar* MaxUpdateRate;
public:
	std::deque<LagRecord> LagRecordsDeck[256];
	LagRecords();
	float GetLerp();
	bool IsTickValid(int tick, int tickcount);
	void CreateMove(CUserCmd_t*&) override;
};

extern std::mutex LagRecordsCriticalSection;