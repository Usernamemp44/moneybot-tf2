#pragma once
#include "../../Misc/Defines.h"
#include "../INetChannel/INetChannel.h"
#include <Windows.h>

class CEngineClient
{
public:
	void GetScreenSize(int &width_out, int &height_out);
	int GetScreenWidth();
	int GetScreenHeight();
	void ServerCmd(const char *szCmdString, bool bReliable = true);
	bool GetPlayerInfo(int nEntityIndex, PlayerInfo_t *pPlayerInfo);
	bool Con_IsVisible(void);
	int GetLocalPlayer(void);
	float Time(void);
	Vec3 GetViewAngles();
	void SetViewAngles(Vec3 &va);
	int GetMaxClients(void);
	bool IsInGame(void);
	bool IsConnected(void);
	bool IsDrawingLoadingImage(void);
	const matrix4x4 & WorldToScreenMatrix(void);
	const matrix4x4 & RealWorldToScreenMatrix(void);
	bool IsTakingScreenshot(void);
	bool IsHLTV();
	INetChannel *GetNetChannelInfo(void);
	void ClientCmd_Unrestricted(const char *szCommandString);
	//const char* GetHostName();
	//const char* GetGamemodeName();
	static CEngineClient* factory();
	static CEngineClient factory_noptr();
};

