#include "IEngineTool.h"
#include "../../Misc/GetInterface.h"
#include "../../../Misc/VMT.h"
IEngineTool* cached_enginetool = nullptr;
void IEngineTool::GetWorldToScreenMatrixForView(const CViewSetup & view, matrix4x4 * pVMatrix)
{
	typedef void(__thiscall *FN)(void*, const CViewSetup & view, matrix4x4 * pVMatrix);
	return GetVFunc<FN>(this, 78)(this, view, pVMatrix);
}
IEngineTool * IEngineTool::factory()
{
	if (cached_enginetool == nullptr) {
		cached_enginetool = (IEngineTool*)(GetInterface("engine.dll", "VENGINETOOL003"));
	}
	return cached_enginetool;
}

