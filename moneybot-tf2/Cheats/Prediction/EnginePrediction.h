#pragma once
#include "../BaseCheat.h"
#include "../../SourceSDK/Misc/Defines.h"
class CBasePlayer;
class EnginePrediction  : BaseCheat {
	public:
		 void StartPrediction(CUserCmd_t*, CBasePlayer*);
		 void EndPrediction(CUserCmd_t*, CBasePlayer*);
		 void FullPredict(CUserCmd_t * cmd, CBasePlayer * player, int tickspredicted = 0, bool finished = false);
		 float oCurTime;
		 float oFrameTime;
		 int* PredictionRandomSeed;
		 CMoveData_t MoveData;
		 EnginePrediction();
};
extern EnginePrediction g_EnginePrediction;