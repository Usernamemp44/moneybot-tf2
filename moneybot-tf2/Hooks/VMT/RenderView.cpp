#include "RenderView.h"
#include "..\Hooks.h"
#include "..\..\Cheats\CheatList.h"
using fn = void(__thiscall *)(void *);
void __fastcall SceneEndHook(void * render_view, int edx)
{
	for (auto &x : CheatModules)
		if (x.second->Callable && !x.second->Priority)
			x.second->SceneEnd();

	RenderViewHook->GetMethod<fn>(SCENE_END_INDEX)(render_view);
	return;
}
