#include "IVDebugOverlay.h"
#include "..\..\Misc\Defines.h"
#include "..\..\Misc\GetInterface.h"
#include "..\..\..\Misc\VMT.h"
IVDebugOverlay* cached_debugoverlay = nullptr;

IVDebugOverlay* IVDebugOverlay::factory() {
	if (cached_debugoverlay == nullptr)
	{
		cached_debugoverlay = (IVDebugOverlay*)(GetInterface("engine.dll", "VDebugOverlay003"));
	}

	return cached_debugoverlay;
}