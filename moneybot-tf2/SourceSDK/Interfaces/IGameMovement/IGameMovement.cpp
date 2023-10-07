#include "IGameMovement.h"

#include "../../../Misc/VMT.h"
#include "../../Misc/GetInterface.h"

#include "../../Entities/CBasePlayer/CBasePlayer.h"
#include "../../Misc/Defines.h"

void IGameMovement::ProcessMovement(CBasePlayer* pPlayer, CMoveData_t* pMoveData) {
	typedef void(__thiscall *FN)(void*, CBasePlayer* pPlayer, CMoveData_t* pMoveData);
	GetVFunc<FN>(this, 1)(this, pPlayer, pMoveData);
}

void IGameMovement::StartTrackPredictionErrors(CBasePlayer* pPlayer) {
	typedef void(__thiscall *FN)(void*, CBasePlayer* pPlayer);
	GetVFunc<FN>(this, 2)(this, pPlayer);
}

void IGameMovement::FinishTrackPredictionErrors(CBasePlayer* pPlayer) {
	typedef void(__thiscall *FN)(void*, CBasePlayer* pPlayer);
	GetVFunc<FN>(this, 3)(this, pPlayer);
}

IGameMovement* cached = nullptr;
IGameMovement* IGameMovement::factory()
{
	if (cached == nullptr) {
		cached = (IGameMovement*)(GetInterface("client.dll", "GameMovement001"));
	}
	return cached;
}
IGameMovement IGameMovement::factory_noptr()
{
	if (cached == nullptr) {
		cached = (IGameMovement*)(GetInterface("client.dll", "GameMovement001"));
	}
	return *cached;
}
