#include "IPanels.h"
#include "../../Misc/GetInterface.h"
#include "../../Misc/Defines.h"
#include "../../../Misc/VMT.h"

const char *IPanel::GetName(unsigned int vgui_panel)
{
	typedef const char *(__thiscall *FN)(PVOID, unsigned int);
	return GetVFunc<FN>(this, 36)(this, vgui_panel);
}

void IPanel::SetMouseInputEnabled(unsigned int panel, bool state)
{
	GetVFunc<void(__thiscall *)(void *, int, bool)>(this, 32)(this, panel, state);
}

void IPanel::SetTopmostPopup(unsigned int panel, bool state)
{
	GetVFunc<void(__thiscall *)(void *, int, bool)>(this, 59)(this, panel, state);
}

IPanel* cached_panel;

IPanel* IPanel::factory() {
	if (!cached_panel) {
		cached_panel = (IPanel*)(GetInterface("vgui2.dll", "VGUI_Panel009"));
	}
	return cached_panel;
}