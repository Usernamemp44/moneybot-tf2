#include "CModelInfo.h"
#include "../../../Misc/VMT.h"
#include "../../Misc/GetInterface.h"

const char *CModelInfo::GetModelName(const DWORD *model)
{
	typedef const char *(__thiscall *FN)(PVOID, const DWORD *);
	return GetVFunc<FN>(this, 3)(this, model);
}

void CModelInfo::GetModelRenderBounds(const DWORD *model, Vec3 &mins, Vec3 &maxs)
{
	typedef void(__thiscall *FN)(PVOID, const DWORD *, Vec3 &, Vec3 &);
	return GetVFunc<FN>(this, 7)(this, model, mins, maxs);
}

DWORD *CModelInfo::GetStudioModel(DWORD *model)
{
	typedef DWORD *(__thiscall *FN)(PVOID, DWORD *);
	return GetVFunc<FN>(this, 28)(this, model);
}
CModelInfo* cached_modelinfo = NULL;
CModelInfo* CModelInfo::factory() {
	if (cached_modelinfo == nullptr) {
		cached_modelinfo = (CModelInfo*)(GetInterface("engine.dll", "VModelInfoClient006"));
	}
	return cached_modelinfo;
}