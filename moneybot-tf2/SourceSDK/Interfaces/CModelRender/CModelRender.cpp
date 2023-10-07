#include "CModelRender.h"
#include "../../../Misc/VMT.h"
#include "..\..\..\Hooks\VMT\ModelRender.h"
#include "../../../SourceSDK/Misc/GetInterface.h"
void CModelRender::ForcedMaterialOverride(IMaterial *mat, OverrideType_t type)
{
	typedef void(__thiscall *FN)(void*, IMaterial *, OverrideType_t);
	return GetVFunc<FN>(this, 1)(this, mat, type);
}

void CModelRender::DrawModelExecute(void *state, ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{
	typedef void(__thiscall *FN)(void*, void *, ModelRenderInfo_t &, matrix3x4 *);
	return GetVFunc<FN>(this, 19)(this, state, pInfo, pCustomBoneToWorld);
}

CModelRender* cached_modelrender = NULL;
CModelRender* CModelRender::factory() {
	if (cached_modelrender == nullptr) {
		cached_modelrender = (CModelRender*)(GetInterface("engine.dll", "VEngineModel016"));
	}
	return cached_modelrender;
}