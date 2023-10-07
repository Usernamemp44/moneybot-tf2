#pragma once
#include <Windows.h>
#include "..\..\SourceSDK\Misc\Defines.h"
#define FRAME_STAGE_NOTIFY_INDEX 35
void __fastcall FrameStageNotifyHook(PVOID client, void* edx, ClientFrameStage frame);