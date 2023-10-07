#include "Surface.h"
#include "../../SourceSDK/Interfaces/ISurface/ISurface.h"
#include "../Hooks.h"

using fn = void(__fastcall *)();
extern bool MenuOpen;
void __fastcall LockCursorHook(void * surface, int edx)
{
	
	SurfaceHook->GetMethod<fn>(LOCK_CURSOR_INDEX)();

	if (MenuOpen)
		ISurface::factory()->UnlockCursor();
	else
		SurfaceHook->GetMethod<fn>(LOCK_CURSOR_INDEX)();
}
