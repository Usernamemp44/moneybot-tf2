#include "Panels.h"
#include "../Hooks.h"
#include "../../SourceSDK/Interfaces/CEngineClient/CEngineClient.h"
#include "../../SourceSDK/Interfaces/IPanels/IPanels.h"
using fn = void(__thiscall *)(void *, unsigned int, bool, bool);
extern matrix4x4 w2smatrix;
void __fastcall PaintTraverseHook(void *panels, int edx, unsigned int vgui_panel, bool force_repaint, bool allow_force)
{
	static unsigned int top_panel;
	PanelsHook->GetMethod<fn>(PAINT_TRAVERSE_INDEX)(panels, vgui_panel, force_repaint, allow_force);
	if (top_panel == 0) {
		const char *name = IPanel::factory()->GetName(vgui_panel);

		if (strstr(name, "FocusOverlayPanel"))
			top_panel = vgui_panel;
	}

	if (top_panel == vgui_panel)
	{
		const matrix4x4& balls = CEngineClient::factory()->RealWorldToScreenMatrix();
		memcpy(w2smatrix, *balls, sizeof(matrix4x4));
	}
	
}
