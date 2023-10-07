#include "CClientModeShared.h"
#include <Windows.h>
#include "../../../Misc/VMT.h"
#include "../CEngineClient/CEngineClient.h"
#include "../CHLClient/CHLClient.h"

bool CClientModeShared::IsChatPanelOutOfFocus(void)
{
	typedef PVOID(__thiscall *FN)(PVOID);
	PVOID CHudChat = GetVFunc<FN>(this, 19)(this);

	if (CHudChat)
		return *reinterpret_cast<float *>((reinterpret_cast<DWORD>(CHudChat) + 0xFC)) == 0.0f;

	return false;
}
CClientModeShared* cached_clientmode = nullptr;
//HANDLE GetModuleHandleA(const char*);
CClientModeShared * CClientModeShared::factory()
{
	if (!cached_clientmode) {
		cached_clientmode = **(CClientModeShared ***)(((*(DWORD**)CHLClient::factory())[10] + 0x5));
	}

	return cached_clientmode;
}

CClientModeShared CClientModeShared::factory_noptr()
{
	return *factory();
}
