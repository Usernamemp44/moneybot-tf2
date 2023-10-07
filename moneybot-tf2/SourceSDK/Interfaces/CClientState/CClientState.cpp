#include "CClientState.h"
#include "../Misc/VMT.h"
#include "../CEngineClient/CEngineClient.h"
#include <stdint.h>

CClientState* cached_clientstate = nullptr;
CClientState* CClientState::factory() {
	if (!cached_clientstate)
		cached_clientstate = (CClientState*)( GetVFunc<uintptr_t>(CEngineClient::factory(), 12) + 0x10);

	return cached_clientstate;
}

CClientState CClientState::factory_noptr() {
	return *cached_clientstate;
}