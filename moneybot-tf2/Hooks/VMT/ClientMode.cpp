#include "ClientMode.h"
#include "../Hooks.h"
#include ".../../../../Cheats/Prediction/EnginePrediction.h"
#include "../../Cheats/CheatList.h"
#include "../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"

#include "../../Cheats/Aimbot/Aimbot.h"

CUserCmd* currentCmd;
extern int nTickBaseShift;
bool __fastcall CreateMoveHook(CClientModeShared * client_mode, void* edx, float input_sample_frametime, CUserCmd * cmd)
{
	currentCmd = cmd;
	CBasePlayer* pPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	Vec3 cmdv = cmd->ViewAngles;

	Vec3 vecViewAngles = cmd->ViewAngles;
	float fSideMove = cmd->SideMove;
	float fForwardMove = cmd->ForwardMove;
	//uintptr_t* frame_pointer;
	//__asm mov frame_pointer, ebp;
	//bool& send_packet = *reinterpret_cast<bool*>(*frame_pointer - 0x1C);

	if (cmd->CommandNumber != 0) {
		g_EnginePrediction.StartPrediction(cmd, pPlayer); {
			for (auto &x : CheatModules) {
				if (x.second->Callable && !x.second->Priority && x.second->ShouldPredict)
					x.second->CreateMove(cmd);
			}
		} g_EnginePrediction.EndPrediction(cmd, pPlayer);

		for (auto &x : CheatModules) {
			if (x.second->Callable && !x.second->Priority && !x.second->ShouldPredict)
				x.second->CreateMove(cmd);
		}
	}
	//if (GetAsyncKeyState('F') & 1)
	//	nTickBaseShift = 198;
	using fn = bool(__thiscall*)(void*, float, CUserCmd *);
	ClientModeHook->GetMethod<fn>(CREATE_MOVE_INDEX)(client_mode, input_sample_frametime, cmd);
	
	//if (cmdv != cmd->ViewAngles)
	//	return true;

	return false;
}


CViewSetup cached_viewsetup;


void __fastcall OverrideViewHook(CClientModeShared *pClientMode, void * edx, CViewSetup *pViewSetup)
{
	//	pViewSetup->fov = 120.f;
	memcpy(&cached_viewsetup, pViewSetup, sizeof(CViewSetup));
	for (auto &x : CheatModules) {
		if (x.second->Callable && !x.second->Priority)
			x.second->OverrideView(pViewSetup);
	}
	ClientModeHook->GetMethod<void(__thiscall *)(void *, CViewSetup *)>(OVERRIDE_VIEW_INDEX)(pClientMode, pViewSetup);

}
