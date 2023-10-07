#define NOMINMAX

#include "LagRecords.h"
#include "../../../CheatList.h"

#include "../../../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"

#include "../../../../Misc/Math.h"

#include <map>
#include <string>
#include <vector>
#include <deque>
#include <array>

// TODO: INVALIDATE TICKS > 64 UNITS APART

float NetWorkLatency() {
	INetChannelInfo* nci = CEngineClient::factory()->GetNetChannelInfo();

	if (nci) {
		return nci->GetAvgLatency(FLOW_INCOMING);
	}
	return 0.0f;
}

int GetTicksChoked(CBasePlayer* player) {
	float simTime = player->GetSimulationTime();
	float simdiff = GlobalVars()->CurTime - simTime;
	float latency = NetWorkLatency();

	return TIME_TO_TICKS(std::max(0.0f, simdiff - latency));
}

LagRecords g_LagRecords;
std::mutex LagRecordsCriticalSection;
LagRecords::LagRecords()
{
	InterpRatio = ICvar::factory()->FindVar("cl_interp_ratio");
	Interp = ICvar::factory()->FindVar("cl_interp");
	MinInterpRatio = ICvar::factory()->FindVar("sv_client_min_interp_ratio");
	MaxInterpRatio = ICvar::factory()->FindVar("sv_client_max_interp_ratio");
	UpdateRate = ICvar::factory()->FindVar("cl_updaterate");
	MaxUpdateRate = ICvar::factory()->FindVar("sv_maxupdaterate");
	CheatModules["LagRecords"] = this;

	//InitializeCriticalSection(&LagRecordsCriticalSection);

	for (int i = 0; i < 256; i++)
		LagRecordsDeck[i] = std::deque<LagRecord>();
}

float LagRecords::GetLerp()
{
	auto ratio = std::clamp(InterpRatio->GetFloat(), MinInterpRatio->GetFloat(), MaxInterpRatio->GetFloat());

	return std::max(Interp->GetFloat(), (ratio / ((MaxUpdateRate->GetFloat()) ? MaxUpdateRate->GetFloat() : UpdateRate->GetFloat())));
}

bool LagRecords::IsTickValid(int tick, int tickcount) {
	float lerp = GetLerp();
	std::clamp<float>(lerp, 0, 0.2f);
	float deltaTime = TICKS_TO_TIME(tickcount - tick);

	if (fabs(deltaTime) > 0.2f)
		return false;
	return true;
}

int TickCount;
void LagRecords::CreateMove(CUserCmd_t *&cmd)
{
	std::lock_guard<std::mutex> guard(LagRecordsCriticalSection);

	TickCount = cmd->TickCount;
	for (int i = 1; i <= CEngineClient::factory()->GetMaxClients(); i++) {

		CBasePlayer* entity = CEntityList::factory()->GetClientPlayer(i);
		if (!entity || !entity->GetLifeState() == LIFE_ALIVE) {
			LagRecordsDeck[i].clear();
			continue;
		}

		if (!LagRecordsDeck[i].empty() && (LagRecordsDeck[i].front().tickcount == cmd->TickCount))
			continue;

		LagRecord record = {};
		record.absorigin = entity->GetAbsOrigin();
		record.vecorigin = entity->GetVecOrigin();
		record.absangles = entity->GetAbsAngles();
		record.eyeangles = entity->GetEyeAngles();
		Math::ClampAngles(record.eyeangles);
		record.tickcount = cmd->TickCount;
		record.velocity = entity->GetVelocity();
		record.chokedticks = GetTicksChoked(entity);
		record.simtime = entity->GetSimulationTime();
		record.hitbox = HITBOX_CHEST; // TODO: Implement aimbot hitbox selection here eventually
		memcpy(record.bonematrix, no_interp_bone_matrix[i], sizeof(matrix3x4) * 256);
		//record.wasvisible = IsEntityVisible(entity);
		LagRecordsDeck[i].push_front(record);
		
		while (LagRecordsDeck[i].size() > 3 && LagRecordsDeck[i].size() > TIME_TO_TICKS(0.2))
			LagRecordsDeck[i].pop_back();
		if (std::deque<LagRecord>::const_iterator invalid = std::find_if(std::cbegin(LagRecordsDeck[i]), std::cend(LagRecordsDeck[i]), [](const LagRecord & rec) { 
			LagRecords* lagrecords = (LagRecords*)CheatModules["LagRecords"];
			return !lagrecords->IsTickValid(rec.tickcount, TickCount);
		}); invalid != std::cend(LagRecordsDeck[i]))
			LagRecordsDeck[i].erase(invalid, std::cend(LagRecordsDeck[i]));
	}
}
