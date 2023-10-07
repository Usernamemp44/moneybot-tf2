#pragma once
#define D3D_DEBUG_INFO
#include <d3d9.h>
#include <Windows.h>
#include "Hooks.h"


extern void * PresentOriginal;
extern void * ResetOriginal;
long __stdcall PresentHook(LPDIRECT3DDEVICE9 pDevice, void* a1, void* a2, HWND a3, void* a4);
HRESULT ResetHook(LPDIRECT3DDEVICE9 pDevice,D3DPRESENT_PARAMETERS *pPresentationParameters);