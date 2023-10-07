#pragma once
#include "../../SourceSDK/Interfaces/IPrediction/IPrediction.h"
#include "../Hooks.h"
#define RUN_COMMAND_INDEX 17
void __fastcall RunCommandHook(IPrediction *prediction, CBaseEntity *entity, CUserCmd *cmd, IMoveHelper *move_helper);
