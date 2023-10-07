#pragma once
#include "..\..\SourceSDK\Misc\Defines.h"
#include "../../SourceSDK/Interfaces/CModelRender/CModelRender.h"
#include "../Hooks.h"
#include "../../Cheats/CheatList.h"
#define DRAW_MODEL_EXECUTE_INDEX 19
void __stdcall DrawModelExecuteHook(DrawModelState_t &state, const ModelRenderInfo_t &info, matrix3x4 *matrix);