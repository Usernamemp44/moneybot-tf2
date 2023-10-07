#pragma once
#include "CEngineClient.h"
#include "..\..\Misc\GetInterface.h"
#include "..\..\..\Misc\VMT.h"
#include "..\..\..\Misc\PatternFinder.h"
#include "..\..\Interfaces\IEngineTool\IEngineTool.h"
int w, h = 0;
void CEngineClient::GetScreenSize(int& w, int& h)
{
	typedef void(__thiscall *FN)(void*, int &, int &);
	GetVFunc<FN>(this, 5)(this, w, h);
	return;
}
int CEngineClient::GetScreenWidth()
{
	GetScreenSize(w, h);
	return w;
}
int CEngineClient::GetScreenHeight()
{
	GetScreenSize(w, h);
	return h;
}
void CEngineClient::ServerCmd(const char *szCmdString, bool bReliable)
{
	typedef void(__thiscall *FN)(void*, const char *, bool);
	return GetVFunc<FN>(this, 6)(this, szCmdString, bReliable);
}

bool CEngineClient::GetPlayerInfo(int nEntityIndex, PlayerInfo_t *pPlayerInfo)
{
	typedef bool(__thiscall *FN)(void*, int, PlayerInfo_t *);
	return GetVFunc<FN>(this, 8)(this, nEntityIndex, pPlayerInfo);
}

bool CEngineClient::Con_IsVisible(void)
{
	typedef bool(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 11)(this);
}

int CEngineClient::GetLocalPlayer(void)
{
	typedef int(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 12)(this);
}

float CEngineClient::Time(void)
{
	typedef float(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 14)(this);
}

Vec3 CEngineClient::GetViewAngles()
{
	Vec3 va;
	typedef void(__thiscall *FN)(void*, Vec3 &va);
	GetVFunc<FN>(this, 19)(this, va);
	return va;
}

void CEngineClient::SetViewAngles(Vec3 &va)
{
	typedef void(__thiscall *FN)(void*, Vec3 &va);
	return GetVFunc<FN>(this, 20)(this, va);
}

int CEngineClient::GetMaxClients(void)
{
	typedef int(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 21)(this);
}

bool CEngineClient::IsInGame(void)
{
	typedef bool(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 26)(this);
}

bool CEngineClient::IsConnected(void)
{
	typedef bool(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 27)(this);
}

bool CEngineClient::IsDrawingLoadingImage(void)
{
	typedef bool(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 28)(this);
}

matrix4x4 w2smatrix;

const matrix4x4 &CEngineClient::WorldToScreenMatrix(void)
{
	return w2smatrix;
}
const matrix4x4 &CEngineClient::RealWorldToScreenMatrix(void)
{
	typedef const matrix4x4 &(__thiscall *FN)(void*);
	const matrix4x4& fuck = GetVFunc<FN>(this, 36)(this);
	matrix4x4 newmc;
	extern CViewSetup cached_viewsetup;
	//IEngineTool::factory()->GetPlayerView(setup, 0, 0, 0, 0);
	//IEngineTool::factory()->GetPlayerView(setup, 0, 0, setup.m_nUnscaledWidth, setup.m_nUnscaledHeight);
	//IEngineTool::factory()->GetWorldToScreenMatrixForView(cached_viewsetup, (matrix4x4*)&newmc);
	return fuck;//GetVFunc<FN>(this, 36)(this);
}

bool CEngineClient::IsTakingScreenshot(void)
{
	typedef bool(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 85)(this);
}

bool CEngineClient::IsHLTV()
{
	typedef bool(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 86)(this);
}

INetChannel *CEngineClient::GetNetChannelInfo(void)
{
	typedef INetChannel *(__thiscall *FN)(void*);
	return GetVFunc<FN>(this, 72)(this);
}

CEngineClient* cached_engine = nullptr;
CEngineClient * CEngineClient::factory()
{
	if (cached_engine == nullptr) {
		cached_engine = (CEngineClient*)(GetInterface("engine.dll", "VEngineClient013"));
	}
	return cached_engine;
}
CEngineClient  CEngineClient::factory_noptr()
{
	if (cached_engine == nullptr) {
		cached_engine = (CEngineClient*)(GetInterface("engine.dll", "VEngineClient013"));
	}
	return *cached_engine;
}

void CEngineClient::ClientCmd_Unrestricted(const char *szCommandString)
{
	typedef void(__thiscall *FN)(void*, const char *);
	return GetVFunc<FN>(this, 106)(this, szCommandString);
}

