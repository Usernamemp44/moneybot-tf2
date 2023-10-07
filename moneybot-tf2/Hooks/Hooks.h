#pragma once
#include "../Misc/VMT.h"
#include "kiero/kiero.h"
#include "minhook/include/MinHook.h"

extern VMTHook* ClientHook;
extern VMTHook* ClientModeHook;
extern VMTHook* PredictionHook;
extern VMTHook* ModelRenderHook;
extern VMTHook* PanelsHook;
extern VMTHook* RenderViewHook;
extern VMTHook* SurfaceHook;
extern VMTHook* InputHook;

void InitializeHooks();
