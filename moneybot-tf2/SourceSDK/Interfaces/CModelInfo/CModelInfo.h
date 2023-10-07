#pragma once
#include <Windows.h>
#include "../../Misc/Defines.h"

class CModelInfo
{
public:
	const char *GetModelName(const DWORD *model);
	void GetModelRenderBounds(const DWORD *model, Vec3 &mins, Vec3 &maxs);
	DWORD *GetStudioModel(DWORD *model);
	static CModelInfo* factory();
};