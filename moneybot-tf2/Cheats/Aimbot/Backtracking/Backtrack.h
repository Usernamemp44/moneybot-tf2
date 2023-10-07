#pragma once
#include "../../BaseCheat.h"
#include "../../Visuals/Chams.h"
#include "LagRecords/LagRecords.h"

struct CUserCmd_t;
enum LegitBacktrackChamType {
	LastTickOnly,
	BestTickOnly,
	AllTicks
};

struct BestTickResult {
	int TickCount;
	CBasePlayer* BestPlayer;
	LagRecord BestRecord;
	bool success;
};

class LegitBacktrack : public BaseCheat{
public:
	bool Enabled = true;
	ChamContainer BacktrackChams;
	LegitBacktrackChamType LegitBacktrackCham;
	BestTickResult GetBestPlayer(CUserCmd_t*& cmd);
	LegitBacktrack();
	void CreateMove( CUserCmd_t*& ) override;
	void DrawPlayer(CBasePlayer* player);
	void SceneEnd () override;
	void RenderMenu() override;
};