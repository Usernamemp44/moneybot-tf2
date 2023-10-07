#include "CHLClient.h"
#include <Windows.h>
#include "..\..\Misc\GetInterface.h"
#include "..\..\..\Misc\VMT.h"
CClientClass *CHLClient::GetAllClasses(void)
{
	typedef CClientClass *(__thiscall *FN)(PVOID);
	return GetVFunc<FN>(this, 8)(this);
}

bool CHLClient::ShouldDrawEntity(CBaseEntity *pEntity)
{
	typedef bool(__thiscall *FN)(PVOID, CBaseEntity *);
	return GetVFunc<FN>(this, 12)(this, pEntity);
}

bool CHLClient::GetPlayerView(CViewSetup & view)
{
	using func_t = bool(__thiscall *)(decltype(this), CViewSetup &);

	return GetVFunc< func_t >(this, 59)(this, view);
}

void CHLClient::RenderView(const CViewSetup & view, int flags, int draw)
{
	using func_t = void(__thiscall *)(decltype(this), const CViewSetup &, int, int);

	GetVFunc< func_t >(this, 27)(this, view, flags, draw);
}
CHLClient* cached_client = nullptr;
CHLClient * CHLClient::factory()
{
	if (cached_client == nullptr) {
		cached_client = (CHLClient*)(GetInterface("client.dll", "VClient017"));
	}
	return cached_client;
}
CHLClient  CHLClient::factory_noptr()
{
	if (cached_client == nullptr) {
		cached_client = (CHLClient*)(GetInterface("client.dll", "VClient017"));
	}
	return *cached_client;
}
