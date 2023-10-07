#include "ISurface.h"
#include "../../../Misc/VMT.h"
#include "../../Misc/GetInterface.h"
void ISurface::UnlockCursor()
{
	typedef void(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 61)(this);
}

void ISurface::LockCursor()
{
	typedef void(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 62)(this);
}
void ISurface::SetCursorAlwaysVisible(bool b)
{
	typedef void(__thiscall *FN)(PVOID, bool);
	GetVFunc<FN>(this, 52)(this, b);
}
ISurface* cached_surface = NULL;
ISurface* ISurface::factory() {
	if (cached_surface == nullptr) {
		cached_surface = (ISurface*)(GetInterface("vguimatsurface.dll", "VGUI_Surface030"));
	}
	return cached_surface;
}