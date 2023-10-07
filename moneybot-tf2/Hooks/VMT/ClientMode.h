#pragma once
#include "../../SourceSDK/Interfaces/CClientModeShared/CClientModeShared.h"

#define CREATE_MOVE_INDEX 21
#define OVERRIDE_VIEW_INDEX 16

bool __fastcall CreateMoveHook(CClientModeShared* client_mode, void* edx, float input_sample_frametime, CUserCmd *cmd);
void __fastcall OverrideViewHook(CClientModeShared *pClientMode, void * edx, CViewSetup *pViewSetup);