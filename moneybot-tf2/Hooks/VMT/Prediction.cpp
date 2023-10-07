#include "Prediction.h"

void __fastcall RunCommandHook(IPrediction * prediction, CBaseEntity * entity, CUserCmd * cmd, IMoveHelper * move_helper)
{
	extern IMoveHelper* cached_IMoveHelper;
	if (move_helper && !cached_IMoveHelper)
		cached_IMoveHelper = move_helper;
	PredictionHook->GetMethod<void(__thiscall *)(void *, CBaseEntity *, CUserCmd *, IMoveHelper *)>(RUN_COMMAND_INDEX)(prediction, entity, cmd, move_helper);
}
