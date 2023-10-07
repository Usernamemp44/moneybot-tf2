#include "EnginePrediction.h"

#include "../../Hooks/VMT/Prediction.h"

#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"

//#include "../../SourceSDK/Interfaces/"
#include "../../SourceSDK/Interfaces/IMoveHelper/IMoveHelper.h"
#include "../../SourceSDK/Interfaces/IPrediction/IPrediction.h"
#include "../../SourceSDK/Interfaces/IPlayerInfoManager/IPlayerInfoManager.h"
#include "../../SourceSDK/Interfaces/IGameMovement/IGameMovement.h"
#include "../../SourceSDK/Interfaces/CEntityList/CEntityList.h"
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"

#include "../../Misc/PatternFinder.h"

#include "../CheatList.h"
// TODO: Make this work with other players.
int flags = 0;
int buttons = 0;

void EnginePrediction::StartPrediction(CUserCmd_t* cmd, CBasePlayer* pPlayer) {
	if (!pPlayer || !cmd || !IMoveHelper::factory())
		return;
	flags = pPlayer->GetFlags();
	//	buttons = cmd->Buttons;
		//if (!PredictionRandomSeed)
		//	PredictionRandomSeed = *reinterpret_cast<int**>(FindPatternInClient("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);

		//*PredictionRandomSeed = cmd->RandomSeed & 0x7FFFFFFF;

		// Store the FrameTime and CurTime so they can be restored later.
	oCurTime = GlobalVars()->CurTime;
	oFrameTime = GlobalVars()->FrameTime;

	// Set CurTime and FrameTime to predicted values
	GlobalVars()->CurTime = pPlayer->GetTickBase() * GlobalVars()->TickInterval;
	GlobalVars()->FrameTime = GlobalVars()->TickInterval;

	pPlayer->SetCurrentCmd(cmd);

	// Start tracking prediction errors
	IGameMovement::factory()->StartTrackPredictionErrors(pPlayer);

	memset(&MoveData, 0, sizeof(MoveData));

	// Set host
	IMoveHelper::factory()->SetHost(pPlayer);

	// Do the movement
	IPrediction::factory()->SetupMove(pPlayer, cmd, IMoveHelper::factory(), &MoveData);
	IGameMovement::factory()->ProcessMovement(pPlayer, &MoveData);
	IPrediction::factory()->FinishMove(pPlayer, cmd, &MoveData);
}

void EnginePrediction::EndPrediction(CUserCmd_t* cmd, CBasePlayer* pPlayer) {
	if (!pPlayer || !cmd || !IMoveHelper::factory())
		return;

	IMoveHelper::factory()->SetHost(nullptr);

	IGameMovement::factory()->FinishTrackPredictionErrors(pPlayer);
	GlobalVars()->CurTime = oCurTime;
	GlobalVars()->FrameTime = oFrameTime;
	//	cmd->Buttons = buttons;
	pPlayer->SetFlags(flags);
	pPlayer->SetCurrentCmd(nullptr);
}
void EnginePrediction::FullPredict(CUserCmd_t* cmd, CBasePlayer* pPlayer, int tickspredicted, bool finish) {
	if (!pPlayer || !cmd || !IMoveHelper::factory())
		return;
	CBasePlayer* pLocalPlayer = CEntityList::factory()->GetClientPlayer(CEngineClient::factory()->GetLocalPlayer());
	flags = pPlayer->GetFlags();
	//	buttons = cmd->Buttons;
		//if (!PredictionRandomSeed)
		//	PredictionRandomSeed = *reinterpret_cast<int**>(FindPatternInClient("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);

		//*PredictionRandomSeed = cmd->RandomSeed & 0x7FFFFFFF;

		// Store the FrameTime and CurTime so they can be restored later.
	oCurTime = GlobalVars()->CurTime;
	oFrameTime = GlobalVars()->FrameTime;

	// Set CurTime and FrameTime to predicted values
	GlobalVars()->CurTime = pLocalPlayer->GetTickBase() * GlobalVars()->TickInterval + (tickspredicted * GlobalVars()->TickInterval);
	GlobalVars()->FrameTime = GlobalVars()->TickInterval;

	pPlayer->SetCurrentCmd(cmd);

	// Start tracking prediction errors
	IGameMovement::factory()->StartTrackPredictionErrors(pPlayer);

	memset(&MoveData, 0, sizeof(MoveData));

	// Set host
	IMoveHelper::factory()->SetHost(pPlayer);

	// Do the movement
	IPrediction::factory()->SetupMove(pPlayer, cmd, IMoveHelper::factory(), &MoveData);
	IGameMovement::factory()->ProcessMovement(pPlayer, &MoveData);
	
	IPrediction::factory()->FinishMove(pPlayer, cmd, &MoveData);
	
	if (!pPlayer || !cmd || !IMoveHelper::factory())
		return;

	IMoveHelper::factory()->SetHost(nullptr);
	
	IGameMovement::factory()->FinishTrackPredictionErrors(pPlayer);
	
	GlobalVars()->CurTime = oCurTime;
	GlobalVars()->FrameTime = oFrameTime;

	//	cmd->Buttons = buttons;
	pPlayer->SetFlags(flags);
	pPlayer->SetCurrentCmd(nullptr);
}
EnginePrediction::EnginePrediction() {
	CheatModules["EnginePrediction"] = this;
}
EnginePrediction g_EnginePrediction;