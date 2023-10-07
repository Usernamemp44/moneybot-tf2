#pragma once
#include "../../SourceSDK/Entities/CBasePlayer/CBasePlayer.h"
extern void* SetupBonesOriginal;
bool __fastcall SetupBonesHook(void* _this, void* edx, matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);