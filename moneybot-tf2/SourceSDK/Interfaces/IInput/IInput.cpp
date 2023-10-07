#include "IInput.h"
#include "../CHLClient/CHLClient.h"
#include <stdint.h>
#include "../../Misc/Defines.h"
#include "../../../Misc/VMT.h"
#include "../../Structures/bitbuf/bitbuf.h"

bool IInput::WriteUsercmdDeltaToBuffer(bf_write * buf, int from, int to, bool isnewcommand)
{
	typedef bool(__thiscall *FN)(void*, bf_write * buf, int from, int to, bool isnewcommand);
	return GetVFunc<FN>(this, 5)(this, buf, from, to, isnewcommand);
}

CUserCmd* IInput::GetUserCmd(int iSequenceNumber) {
	typedef CUserCmd*(__thiscall *FN)(void*, int);
	return GetVFunc<FN>(this, 8)(this, iSequenceNumber);
}

IInput* cached_input = nullptr;

IInput* IInput::factory()
{
	if (!cached_input) {
		cached_input = **(IInput ***)(((*(uint32_t**)CHLClient::factory())[16] + 0x2));
	}

	return cached_input;
}

IInput IInput::factory_noptr()
{
	return *factory();
}